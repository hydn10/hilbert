#include <hilbertcli/cli.hpp>

#include <hilbertcli/output.hpp>
#include <hilbertcli/process/exit_status.hpp>
#include <hilbertcli/simulation.hpp>

#include <hilbert/simulation/core/settings.hpp>
#include <hilbert/simulation/drivers/run.hpp>
#include <hilbert/simulation/suspension/factory.hpp>
#include <hilbert/simulation/suspension/parameters.hpp>
#include <hilbert/simulation/suspension/sinks/soa_vector.hpp>
#include <hilbert/simulation/suspension/state.hpp>

#include <charconv>
#include <cmath>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
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


namespace hilbertcli
{
namespace
{

struct simulation_command
{
  hilbert::simulation::simulation_settings<double> settings{
      .time_step = 0.0005,
      .duration = 20.0,
  };

  hilbert::simulation::suspension::parameters<double> parameters{
      .sprung_mass = 270,
      .unsprung_mass = 30,
      .suspension_spring_constant = 31000,
      .suspension_damping_coefficient = 350,
      .tire_spring_constant = 196000,
      .ground_amplitude = 0.003,
  };

  std::optional<std::filesystem::path> output_path;
};


struct help_command
{
};


using command = std::variant<simulation_command, help_command>;


void
print_usage_synopsis(std::ostream &output)
{
  std::print(output, "Usage: hilbert-cli [--output PATH] [--duration SECONDS] [--time-step SECONDS]\n");
}


void
print_usage(std::ostream &output)
{
  print_usage_synopsis(output);
  std::print(output, "\nWrite the suspension simulation as multi-table text. Output defaults to stdout.\n");
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


command
parse_command(std::span<char const *const> arguments)
{
  simulation_command simulation;

  for (auto const &option_tokens : arguments | std::views::drop(1) | std::views::chunk(2))
  {
    auto current_argument = option_tokens.begin();
    std::string_view const argument{*current_argument++};

    if (argument == "--help" || argument == "-h")
    {
      return help_command{};
    }

    if (current_argument == option_tokens.end())
    {
      throw std::invalid_argument{std::format("missing value for {}", argument)};
    }

    std::string_view const value{*current_argument++};
    if (argument == "--output")
    {
      simulation.output_path.emplace(value.begin(), value.end());
    }
    else if (argument == "--duration")
    {
      simulation.settings.duration = parse_positive_double(value, argument);
    }
    else if (argument == "--time-step")
    {
      simulation.settings.time_step = parse_positive_double(value, argument);
    }
    else
    {
      throw std::invalid_argument{std::format("unknown option: {}", argument)};
    }
  }

  if (simulation.settings.duration < simulation.settings.time_step)
  {
    throw std::invalid_argument{"duration must be at least one time step"};
  }

  return simulation;
}


struct command_dispatcher
{
  void
  operator()(simulation_command const &command) const
  {
    auto const simulate_to = [&command](std::ostream &output)
    {
      auto samples = hilbert::simulation::run_simulation(
          hilbert::simulation::suspension::make_simulation(
              command.settings,
              command.parameters,
              frequency_profile<double>{},
              hilbert::simulation::suspension::state<double>{0, 0, 0, 0, 0}),
          hilbert::simulation::suspension::sinks::soa_vector_sink_factory<double>{});

      auto const analysis = analyze_simulation(command.settings, samples);
      write_simulation_data(output, samples, analysis);
    };

    if (command.output_path)
    {
      std::ofstream output{*command.output_path};
      if (!output)
      {
        throw std::runtime_error{"could not open output file: " + command.output_path->string()};
      }
      simulate_to(output);
    }
    else
    {
      simulate_to(std::cout);
    }
  }

  void
  operator()([[maybe_unused]] help_command const &help) const
  {
    print_usage(std::cout);
  }
};

} // namespace


cli_result
run_cli(std::span<char const *const> arguments)
try
{
  auto const parsed_command = parse_command(arguments);

  std::visit(command_dispatcher{}, parsed_command);

  return outcome::success{};
}
catch (std::exception const &error)
{
  std::println(std::cerr, "hilbert-cli: {}", error.what());
  print_usage_synopsis(std::cerr);

  return outcome::cli::error{};
}

} // namespace hilbertcli
