#include <hilbert_freq_sweep/analysis.hpp>
#include <hilbert_freq_sweep/output.hpp>

#include <hilbert/simulation/core/settings.hpp>
#include <hilbert/simulation/drivers/run.hpp>
#include <hilbert/simulation/suspension/factory.hpp>
#include <hilbert/simulation/suspension/ground_frequencies/constant.hpp>
#include <hilbert/simulation/suspension/parameters.hpp>
#include <hilbert/simulation/suspension/sinks/soa_vector.hpp>
#include <hilbert/simulation/suspension/state.hpp>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <variant>
#include <vector>


namespace hilbert_freq_sweep
{
namespace
{

constexpr double default_start_frequency_hz = 1.0;
constexpr double default_end_frequency_hz = 25.0;
constexpr double default_frequency_step_hz = 1.0;
constexpr double simulation_time_step_s = 0.0005;
constexpr double simulation_duration_s = 15.0;


struct frequency_sweep_command
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
      "Usage: hilbert-frequency-sweep [--output PATH] [--start-frequency HZ] [--end-frequency HZ] "
      "[--frequency-step HZ]\n");
}


void
print_usage(std::ostream &output)
{
  print_usage_synopsis(output);
  std::print(
      output,
      "\nRun independent 15-second constant-frequency suspension simulations and write phase results.\n"
      "Defaults: 1 Hz through 25 Hz in 1 Hz steps; output defaults to stdout.\n");
}


double
parse_positive_double(std::string_view value, std::string_view option)
{
  double result{};
  auto const *const value_begin = std::to_address(value.begin());
  auto const *const value_end = std::to_address(value.end());
  auto const [parsed_end, error] = std::from_chars(value_begin, value_end, result);

  if (error != std::errc{} || parsed_end != value_end || !std::isfinite(result) || result <= 0)
  {
    throw std::invalid_argument{std::string{option} + " requires a finite, positive number"};
  }

  return result;
}


std::variant<frequency_sweep_command, help_command>
parse_command(std::span<char const *const> arguments)
{
  frequency_sweep_command command;

  for (auto const &option_tokens : arguments | std::views::drop(1) | std::views::chunk(2))
  {
    auto current_argument = option_tokens.begin();
    std::string_view const option{*current_argument++};
    if (option == "--help" || option == "-h")
    {
      return help_command{};
    }

    if (current_argument == option_tokens.end())
    {
      throw std::invalid_argument{std::format("missing value for {}", option)};
    }

    std::string_view const value{*current_argument++};
    if (option == "--output")
    {
      command.output_path.emplace(value.begin(), value.end());
    }
    else if (option == "--start-frequency")
    {
      command.start_frequency_hz = parse_positive_double(value, option);
    }
    else if (option == "--end-frequency")
    {
      command.end_frequency_hz = parse_positive_double(value, option);
    }
    else if (option == "--frequency-step")
    {
      command.frequency_step_hz = parse_positive_double(value, option);
    }
    else
    {
      throw std::invalid_argument{std::format("unknown option: {}", option)};
    }
  }

  if (command.end_frequency_hz < command.start_frequency_hz)
  {
    throw std::invalid_argument{"end frequency must not be lower than start frequency"};
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


frequency_sweep_result<double>
simulate_frequency_sweep_point(double frequency_hz)
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
      .phase_fit_rad = estimate_frequency_sweep_phase_by_least_squares<double>(samples),
      .phase_hilbert_rad = estimate_frequency_sweep_phase_by_hilbert_transform<double>(samples),
  };
}


std::vector<frequency_sweep_result<double>>
run_frequency_sweep(frequency_sweep_command const &command)
{
  std::vector<frequency_sweep_result<double>> results;
  auto const frequency_tolerance = 8 * std::numeric_limits<double>::epsilon() *
                                   std::max({1.0, std::abs(command.start_frequency_hz), command.end_frequency_hz});

  for (size_t index = 0uz;; ++index)
  {
    auto const frequency = command.start_frequency_hz + command.frequency_step_hz * static_cast<double>(index);
    if (frequency > command.end_frequency_hz + frequency_tolerance)
    {
      break;
    }
    results.emplace_back(simulate_frequency_sweep_point(frequency));
  }

  return results;
}


void
run_frequency_sweep_command(frequency_sweep_command const &command)
{
  auto results = run_frequency_sweep(command);
  auto write_to = [&results](std::ostream &output)
  {
    write_frequency_sweep_results(output, std::span<frequency_sweep_result<double> const>{results});
  };

  if (command.output_path)
  {
    std::ofstream output{*command.output_path};
    if (!output)
    {
      throw std::runtime_error{"could not open output file: " + command.output_path->string()};
    }
    write_to(output);
  }
  else
  {
    write_to(std::cout);
  }
}

} // namespace

} // namespace hilbert_freq_sweep


namespace
{

int
execute_application(std::span<char const *const> arguments)
try
{
  auto const command = hilbert_freq_sweep::parse_command(arguments);
  if (auto const *const sweep = std::get_if<0>(&command))
  {
    hilbert_freq_sweep::run_frequency_sweep_command(*sweep);
  }
  else
  {
    hilbert_freq_sweep::print_usage(std::cout);
  }
  return 0;
}
catch (std::exception const &error)
{
  std::println(std::cerr, "hilbert-frequency-sweep: {}", error.what());
  hilbert_freq_sweep::print_usage_synopsis(std::cerr);
  return 2;
}


int
run_application(std::span<char const *const> arguments) noexcept
try
{
  return execute_application(arguments);
}
catch (...)
{
  return 1;
}

} // namespace


int
main(int argc, char const **argv)
{
  auto const arguments = std::span<char const *const>{argv, static_cast<size_t>(argc)};
  return run_application(arguments);
}
