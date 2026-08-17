#include <hilbert/egea/egea.hpp>

#include <hilbert/app/cli/arguments.hpp>
#include <hilbert/app/cli/parse.hpp>
#include <hilbert/app/cli/run.hpp>
#include <hilbert/app/io/output_stream.hpp>
#include <hilbert/egea/output.hpp>
#include <hilbert/egea/simulation.hpp>
#include <hilbert/simulation.hpp>

#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <print>
#include <span>
#include <stdexcept>
#include <string_view>
#include <variant>


namespace hilbert::egea
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
  std::print(output, "Usage: hilbert-egea [--output PATH] [--duration SECONDS] [--time-step SECONDS]\n");
}


void
print_usage(std::ostream &output)
{
  print_usage_synopsis(output);
  std::print(output, "\nWrite the suspension simulation as multi-table text. Output defaults to stdout.\n");
}


command
parse_command(std::span<char const *const> arguments)
{
  simulation_command simulation;
  auto options = hilbert::app::cli::argument_cursor{arguments.subspan(1)};

  while (options)
  {
    auto const argument = options.next();

    if (argument == "--help" || argument == "-h")
    {
      return help_command{};
    }

    auto const value = options.require_value(argument);
    if (argument == "--output")
    {
      simulation.output_path.emplace(value.begin(), value.end());
    }
    else if (argument == "--duration")
    {
      simulation.settings.duration = hilbert::app::cli::parse_positive_double(value, argument);
    }
    else if (argument == "--time-step")
    {
      simulation.settings.time_step = hilbert::app::cli::parse_positive_double(value, argument);
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

    hilbert::app::io::with_output_stream(command.output_path, std::cout, simulate_to);
  }

  void
  operator()([[maybe_unused]] help_command const &help) const
  {
    print_usage(std::cout);
  }
};

} // namespace


egea_result
run_cli(std::span<char const *const> arguments)
{
  return hilbert::app::cli::run(
      "hilbert-egea",
      std::cerr,
      [&arguments]
      {
        auto const parsed_command = parse_command(arguments);
        std::visit(command_dispatcher{}, parsed_command);
      },
      print_usage_synopsis);
}

} // namespace hilbert::egea
