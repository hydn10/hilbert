#include <hilbertcli/cli.hpp>

#include <hilbertcli/exit_status.hpp>
#include <hilbertcli/output.hpp>
#include <hilbertcli/simulation.hpp>

#include <cmath>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>


namespace hilbertcli
{
namespace
{

struct simulation_command
{
  simulation_config simulation;
  std::optional<std::filesystem::path> output_path;
};


struct help_command
{
};


using command = std::variant<simulation_command, help_command>;


void
print_usage(std::ostream &output)
{
  std::print(
      output,
      "Usage: hilbert-cli [--output PATH] [--duration SECONDS] [--time-step SECONDS]\n"
      "\n"
      "Write the suspension simulation as multi-table text. Output defaults to stdout.\n");
}


double
parse_positive_double(std::string const &value, std::string_view option)
{
  size_t parsed_characters = 0;
  auto const result = std::stod(value, &parsed_characters);
  if (parsed_characters != value.size() || !std::isfinite(result) || result <= 0)
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
    std::string const argument{*current_argument++};

    if (argument == "--help" || argument == "-h")
    {
      return help_command{};
    }

    if (current_argument == option_tokens.end())
    {
      throw std::invalid_argument{"missing value for " + argument};
    }

    std::string const value{*current_argument++};
    if (argument == "--output")
    {
      simulation.output_path = value;
    }
    else if (argument == "--duration")
    {
      simulation.simulation.duration = parse_positive_double(value, argument);
    }
    else if (argument == "--time-step")
    {
      simulation.simulation.time_step = parse_positive_double(value, argument);
    }
    else
    {
      throw std::invalid_argument{"unknown option: " + argument};
    }
  }

  if (simulation.simulation.duration < simulation.simulation.time_step)
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
      auto const result = run_simulation(command.simulation);
      write_simulation_data(output, result);
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


exit_status
run_cli(int argc, char const **argv)
{
  try
  {
    auto const arguments = std::span<char const *const>{argv, static_cast<size_t>(argc)};
    auto const parsed_command = parse_command(arguments);
    std::visit(command_dispatcher{}, parsed_command);
  }
  catch (std::exception const &error)
  {
    std::println(std::cerr, "hilbert-cli: {}", error.what());
    std::println(std::cerr, "Usage: hilbert-cli [--output PATH] [--duration SECONDS] [--time-step SECONDS]");
    return exit_status::cli_error;
  }

  return exit_status::success;
}

} // namespace hilbertcli
