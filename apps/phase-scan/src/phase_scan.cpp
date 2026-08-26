#include <hilbert/phase_scan/phase_scan.hpp>

#include <hilbert/analysis/least_squares/least_squares_residual_statistics.hpp>
#include <hilbert/analysis/least_squares/observation.hpp>
#include <hilbert/analysis/sampling/sample_window.hpp>
#include <hilbert/analysis/sinusoidal/basis.hpp>
#include <hilbert/analysis/sinusoidal/frequency_hz.hpp>
#include <hilbert/app/cli/argument_cursor.hpp>
#include <hilbert/app/cli/error.hpp>
#include <hilbert/app/cli/parse.hpp>
#include <hilbert/app/cli/run.hpp>
#include <hilbert/app/io/output_stream.hpp>
#include <hilbert/app/process/exit_status.hpp>
#include <hilbert/phase_scan/estimators/hilbert_estimate.hpp>
#include <hilbert/phase_scan/estimators/least_squares_estimate.hpp>
#include <hilbert/phase_scan/output.hpp>
#include <hilbert/phase_scan/result.hpp>
#include <hilbert/simulation.hpp>
#include <hilbert/simulation/sinks/adapters.hpp>
#include <hilbert/simulation/sinks/normal_equations.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <limits>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>


namespace hilbert::phase_scan
{
namespace
{

constexpr double default_start_frequency_hz = 6.0;
constexpr double default_end_frequency_hz = 18.0;
constexpr double default_frequency_step_hz = 1.0;

constexpr double simulation_time_step_s = 0.0005;
constexpr double simulation_duration_s = 20.0;

constexpr double measurement_start_s = 10.0;
constexpr double measurement_end_s = 15.0;


struct phase_scan_command
{
  double start_frequency_hz = default_start_frequency_hz;
  double end_frequency_hz = default_end_frequency_hz;
  double frequency_step_hz = default_frequency_step_hz;

  std::optional<std::filesystem::path> output_path;
};


struct help_command
{
};


void
print_usage_synopsis(std::ostream &output)
{
  std::print(
      output,
      "Usage: hilbert-phase-scan [--output PATH] [--start-frequency HZ] [--end-frequency HZ] "
      "[--frequency-step HZ]\n");
}


void
print_usage(std::ostream &output)
{
  print_usage_synopsis(output);
  std::print(
      output,
      "\nRun independent {}-second constant-frequency suspension simulations and write frequency-response results.\n"
      "Defaults: {} Hz through {} Hz in {} Hz steps. Output defaults to stdout.\n",
      simulation_duration_s,
      default_start_frequency_hz,
      default_end_frequency_hz,
      default_frequency_step_hz);
}


std::variant<phase_scan_command, help_command>
parse_command(std::span<char const *const> arguments)
{
  phase_scan_command command;
  auto options = hilbert::app::cli::argument_cursor{arguments.subspan(1)};

  while (options)
  {
    auto const option = options.next();
    if (option == "--help" || option == "-h")
    {
      return help_command{};
    }

    auto const value = options.require_value(option);
    if (option == "--output")
    {
      command.output_path.emplace(value.begin(), value.end());
    }
    else if (option == "--start-frequency")
    {
      command.start_frequency_hz = hilbert::app::cli::require_positive_double(value, option);
    }
    else if (option == "--end-frequency")
    {
      command.end_frequency_hz = hilbert::app::cli::require_positive_double(value, option);
    }
    else if (option == "--frequency-step")
    {
      command.frequency_step_hz = hilbert::app::cli::require_positive_double(value, option);
    }
    else
    {
      throw hilbert::app::cli::error{std::format("unknown option: {}", option)};
    }
  }

  if (command.end_frequency_hz < command.start_frequency_hz)
  {
    throw hilbert::app::cli::error{"end frequency must not be lower than start frequency"};
  }

  return command;
}


hilbert::simulation::suspension::parameters<double>
default_parameters()
{
  return {
      .sprung_mass = 270,
      .unsprung_mass = 30,
      .suspension_spring_constant = 31000,
      .suspension_damping_coefficient = 350,
      .tire_spring_constant = 196000,
      .ground_amplitude = 0.003,
  };
}


result<double>
simulate_phase_scan_point(double frequency_hz)
{
  using namespace hilbert::simulation;
  using namespace hilbert::simulation::suspension;

  simulation_settings<double> const settings{
      .time_step = simulation_time_step_s,
      .duration = simulation_duration_s,
  };
  auto const measurement_window = hilbert::analysis::time_window{measurement_start_s, measurement_end_s};
  auto const frequency = hilbert::analysis::frequency_hz{frequency_hz};

  auto const basis = hilbert::analysis::make_sinusoidal_basis(frequency);
  auto const make_fit_observation = [](suspension::sample<double> sample)
  {
    return hilbert::analysis::make_observation(sample.time, sample.ground_displacement, sample.tire_force);
  };
  auto const predicate = [measurement_window](suspension::sample<double> const &sample)
  {
    return sample.time >= measurement_window.begin() && sample.time < measurement_window.end();
  };

  auto const fit_sink_factory = hilbert::simulation::sinks::make_filtered_sink_factory(
      hilbert::simulation::sinks::make_normal_equations_sink_factory<double, 2uz>(
          basis, make_fit_observation, hilbert::analysis::least_squares_residual_statistics_collector<double, 2uz>{}),
      predicate);
  auto const sink_factory = hilbert::simulation::sinks::make_tee_sink_factory(
      suspension::sinks::soa_vector_sink_factory<double>{}, fit_sink_factory);

  auto const [samples, products] = run_simulation(
      suspension::make_simulation(
          settings, default_parameters(), ground_frequencies::constant{frequency_hz}, state<double>{0, 0, 0, 0, 0}),
      sink_factory);

  auto const measurement = hilbert::analysis::select_sample_range(samples.time_span(), measurement_window);

  auto const least_squares = estimate_phase_scan_by_least_squares(products);
  auto const hilbert_estimate = estimate_phase_scan_by_hilbert_transform(
      samples.ground_displacement_span(), samples.tire_force_span(), measurement);
  auto const &[basis_condition_number, ground_normalized_residual, tire_force_normalized_residual] =
      least_squares.diagnostics();
  auto const &[mean_resultant_length, gain_coefficient_of_variation] = hilbert_estimate.diagnostics();

  return {
      .frequency_hz = frequency_hz,
      .magnitude_least_squares_n_per_m = least_squares.response().magnitude(),
      .magnitude_hilbert_n_per_m = hilbert_estimate.response().magnitude(),
      .phase_least_squares_rad = least_squares.response().phase().radians(),
      .phase_hilbert_rad = hilbert_estimate.response().phase().radians(),
      .least_squares_basis_condition_number = basis_condition_number,
      .least_squares_ground_normalized_residual = ground_normalized_residual,
      .least_squares_tire_force_normalized_residual = tire_force_normalized_residual,
      .hilbert_mean_resultant_length = mean_resultant_length.value(),
      .hilbert_gain_coefficient_of_variation = gain_coefficient_of_variation,
  };
}


std::size_t
phase_scan_point_count(phase_scan_command const &command)
{
  auto const steps = (command.end_frequency_hz - command.start_frequency_hz) / command.frequency_step_hz;
  auto const step_tolerance = 8 * std::numeric_limits<double>::epsilon() * std::max(1.0, std::abs(steps));

  return static_cast<std::size_t>(std::floor(steps + step_tolerance)) + 1uz;
}


std::vector<result<double>>
run_phase_scan(phase_scan_command const &command)
{
  auto const frequencies = std::views::iota(0uz, phase_scan_point_count(command)) |
                           std::views::transform(
                               [start = command.start_frequency_hz, step = command.frequency_step_hz](std::size_t index)
                               {
                                 return std::fma(step, static_cast<double>(index), start);
                               });

  return frequencies | std::views::transform(simulate_phase_scan_point) | std::ranges::to<std::vector>();
}


void
run_phase_scan_command(phase_scan_command const &command)
{
  auto const results = run_phase_scan(command);

  auto write_to = [&results](std::ostream &output)
  {
    write_results(output, std::span<result<double> const>{results});
  };

  hilbert::app::io::with_output_stream(command.output_path, std::cout, std::move(write_to));
}


struct command_dispatcher
{
  void
  operator()(phase_scan_command const &command) const
  {
    run_phase_scan_command(command);
  }

  void
  operator()([[maybe_unused]] help_command const &help) const
  {
    print_usage(std::cout);
  }
};

} // namespace


hilbert::app::application_result
run_cli(std::span<char const *const> arguments)
{
  return hilbert::app::cli::run(
      "hilbert-phase-scan",
      std::cerr,
      [&arguments]
      {
        auto const parsed_command = parse_command(arguments);
        std::visit(command_dispatcher{}, parsed_command);
      },
      print_usage_synopsis);
}

} // namespace hilbert::phase_scan
