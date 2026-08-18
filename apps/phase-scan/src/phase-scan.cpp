#include <hilbert/phase_scan/phase_scan.hpp>

#include <hilbert/app/cli/arguments.hpp>
#include <hilbert/app/cli/error.hpp>
#include <hilbert/app/cli/parse.hpp>
#include <hilbert/app/cli/run.hpp>
#include <hilbert/app/io/output_stream.hpp>
#include <hilbert/app/process/exit_status.hpp>
#include <hilbert/phase_scan/analysis.hpp>
#include <hilbert/phase_scan/output.hpp>
#include <hilbert/simulation.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <limits>
#include <optional>
#include <print>
#include <span>
#include <string_view>
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

constexpr auto measurement_window = hilbert::analysis::time_window<double>{
    .begin = measurement_start_s,
    .end = measurement_end_s,
};


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
      "\nRun independent {}-second constant-frequency suspension simulations and write phase results.\n"
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


phase_scan_result<double>
simulate_phase_scan_point(double frequency_hz)
{
  using namespace hilbert::simulation;
  using namespace hilbert::simulation::suspension;

  simulation_settings<double> const settings{
      .time_step = simulation_time_step_s,
      .duration = simulation_duration_s,
  };

  auto const samples = run_simulation(
      suspension::make_simulation(
          settings,
          default_parameters(),
          ground_frequencies::constant<double>{frequency_hz},
          state<double>{0, 0, 0, 0, 0}),
      suspension::sinks::soa_vector_sink_factory<double>{});

  return {
      .frequency_hz = frequency_hz,
      .phase_fit_rad = estimate_phase_scan_by_least_squares<double>(samples, frequency_hz, measurement_window),
      .phase_hilbert_rad = estimate_phase_scan_by_hilbert_transform<double>(samples, measurement_window),
  };
}


std::vector<phase_scan_result<double>>
run_phase_scan(phase_scan_command const &command)
{
  std::vector<phase_scan_result<double>> results;
  auto const frequency_tolerance = 8 * std::numeric_limits<double>::epsilon() *
                                   std::max({1.0, std::abs(command.start_frequency_hz), command.end_frequency_hz});

  for (size_t index = 0uz;; ++index)
  {
    auto const frequency = command.start_frequency_hz + command.frequency_step_hz * static_cast<double>(index);
    if (frequency > command.end_frequency_hz + frequency_tolerance)
    {
      break;
    }
    results.emplace_back(simulate_phase_scan_point(frequency));
  }

  return results;
}


void
run_phase_scan_command(phase_scan_command const &command)
{
  auto const results = run_phase_scan(command);

  auto write_to = [&results](std::ostream &output)
  {
    write_phase_scan_results(output, std::span<phase_scan_result<double> const>{results});
  };

  hilbert::app::io::with_output_stream(command.output_path, std::cout, write_to);
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
