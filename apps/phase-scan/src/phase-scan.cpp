#include <hilbert/phase_scan/phase_scan.hpp>

#include <hilbert/app/process/exit_status.hpp>
#include <hilbert/phase_scan/analysis.hpp>
#include <hilbert/phase_scan/output.hpp>
#include <hilbert/simulation.hpp>

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


namespace hilbert::phase_scan
{
namespace
{

constexpr double default_start_frequency_hz = 6.0;
constexpr double default_end_frequency_hz = 18.0;
constexpr double default_frequency_step_hz = 1.0;
constexpr double simulation_time_step_s = 0.0005;
constexpr double simulation_duration_s = 20.0;


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
      "\nRun independent 15-second constant-frequency suspension simulations and write phase results.\n"
      "Defaults: 1 Hz through 25 Hz in 1 Hz steps. Output defaults to stdout.\n");
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


std::variant<phase_scan_command, help_command>
parse_command(std::span<char const *const> arguments)
{
  phase_scan_command command;

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
      .phase_fit_rad = estimate_phase_scan_by_least_squares<double>(samples),
      .phase_hilbert_rad = estimate_phase_scan_by_hilbert_transform<double>(samples),
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
  auto results = run_phase_scan(command);
  auto write_to = [&results](std::ostream &output)
  {
    write_phase_scan_results(output, std::span<phase_scan_result<double> const>{results});
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
hilbert::app::application_result
run_cli(std::span<char const *const> arguments)
try
{
  auto const command = parse_command(arguments);
  if (auto const *const scan = std::get_if<0>(&command))
  {
    run_phase_scan_command(*scan);
  }
  else
  {
    print_usage(std::cout);
  }
  return hilbert::app::outcome::success{};
}
catch (std::exception const &error)
{
  std::println(std::cerr, "hilbert-phase-scan: {}", error.what());
  print_usage_synopsis(std::cerr);
  return hilbert::app::outcome::application::error{};
}

} // namespace hilbert::phase_scan
