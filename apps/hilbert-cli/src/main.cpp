#include <collectors.hpp>
#include <derivative.hpp>
#include <rk4.hpp>
#include <state.hpp>

#include <hilbert/hilbert.hpp>

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>


namespace
{

double constexpr measurement_start_time = 7.0;
double constexpr measurement_end_time = 16.0;

struct simulation_command
{
  double time_step = 0.0005;
  double duration = 20.0;
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
      simulation.duration = parse_positive_double(value, argument);
    }
    else if (argument == "--time-step")
    {
      simulation.time_step = parse_positive_double(value, argument);
    }
    else
    {
      throw std::invalid_argument{"unknown option: " + argument};
    }
  }

  if (simulation.duration < simulation.time_step)
  {
    throw std::invalid_argument{"duration must be at least one time step"};
  }

  return simulation;
}


template<std::floating_point Float>
Float
ground_frequency(Float time)
{
  Float constexpr summit_time = 1.5;
  Float constexpr descent_time = 6;
  Float constexpr test_end_time = 18.5;

  Float constexpr start_frequency = 0;
  Float constexpr summit_frequency = 25;
  Float constexpr measure_start_frequency = 18;
  Float constexpr measure_end_frequency = 6;
  Float constexpr end_frequency = 0;

  auto constexpr make_slope = [](Float begin_time, Float finish_time, Float initial_frequency, Float final_frequency)
  {
    return [=](Float value)
    {
      return initial_frequency +
             (final_frequency - initial_frequency) / (finish_time - begin_time) * (value - begin_time);
    };
  };

  auto constexpr initial_slope = make_slope(0, summit_time, start_frequency, summit_frequency);
  auto constexpr descent_slope =
      make_slope(descent_time, measurement_start_time, summit_frequency, measure_start_frequency);
  auto constexpr measure_slope =
      make_slope(measurement_start_time, measurement_end_time, measure_start_frequency, measure_end_frequency);
  auto constexpr wind_down_slope =
      make_slope(measurement_end_time, test_end_time, measure_end_frequency, end_frequency);

  if (time < summit_time)
  {
    return initial_slope(time);
  }
  if (time < descent_time)
  {
    return summit_frequency;
  }
  if (time < measurement_start_time)
  {
    return descent_slope(time);
  }
  if (time < measurement_end_time)
  {
    return measure_slope(time);
  }
  if (time < test_end_time)
  {
    return wind_down_slope(time);
  }
  return end_frequency;
}


template<std::floating_point Float>
constexpr auto
make_ground_position_function(Float amplitude)
{
  return [amplitude](Float phase)
  {
    return amplitude * std::sin(phase);
  };
}


template<std::floating_point Float>
constexpr auto
make_state_derivative_function(
    Float sprung_mass,
    Float unsprung_mass,
    Float suspension_spring_constant,
    Float suspension_damping_coefficient,
    Float tire_spring_constant,
    auto ground_position_function)
{
  return [=](Float time, hilbertcli::state<Float> const &state) -> hilbertcli::derivative<Float>
  {
    Float const phase_velocity = 2 * std::numbers::pi * ground_frequency(time);
    Float const platform_position = ground_position_function(state.phi());
    Float const sprung_acceleration = (-suspension_damping_coefficient * (state.vs() - state.vu()) -
                                       suspension_spring_constant * (state.xs() - state.xu())) /
                                      sprung_mass;
    Float const unsprung_acceleration = (suspension_damping_coefficient * (state.vs() - state.vu()) +
                                         suspension_spring_constant * (state.xs() - state.xu()) -
                                         tire_spring_constant * (state.xu() - platform_position)) /
                                        unsprung_mass;

    return hilbertcli::derivative{
        phase_velocity,
        state.vs(),
        state.vu(),
        sprung_acceleration,
        unsprung_acceleration,
    };
  };
}


void
write_simulation_data(
    std::ostream &output,
    hilbertcli::vec_collector const &collector,
    std::span<double const> refined_time,
    hilbert::signal_data<double> const &platform_signal,
    hilbert::signal_data<double> const &tire_force_signal)
{
  output << std::setprecision(17);

  auto const time = collector.time_span();
  auto const sprung = collector.xs_span();
  auto const unsprung = collector.xu_span();
  auto const platform = collector.ground_span();
  auto const tire_force = collector.tire_force_span();

  output << "# table: raw\n"
            "time_s,sprung_displacement_m,unsprung_displacement_m,platform_displacement_m,tire_force_n\n";

  for (
      auto const &[time_value, sprung_value, unsprung_value, platform_value, tire_force_value] :
      std::views::zip(time, sprung, unsprung, platform, tire_force))
  {
    output << time_value << ',' << sprung_value << ',' << unsprung_value << ',' << platform_value << ','
           << tire_force_value << '\n';
  }

  output << "\n# table: refined\n"
            "time_s,platform_amplitude_m,platform_phase_rad,platform_frequency_hz,tire_force_amplitude_n,"
            "tire_force_phase_rad,tire_force_frequency_hz\n";

  for (
      auto const
          &[time_value,
            platform_amplitude,
            platform_phase,
            platform_frequency,
            tire_force_amplitude,
            tire_force_phase,
            tire_force_frequency] :
      std::views::zip(
          refined_time,
          platform_signal.ampl,
          platform_signal.phase,
          platform_signal.freq,
          tire_force_signal.ampl,
          tire_force_signal.phase,
          tire_force_signal.freq))
  {
    output << time_value << ',' << platform_amplitude << ',' << platform_phase << ',' << platform_frequency << ','
           << tire_force_amplitude << ',' << tire_force_phase << ',' << tire_force_frequency << '\n';
  }
}


void
run_simulation(simulation_command const &simulation, std::ostream &output)
{
  if (simulation.duration < measurement_end_time)
  {
    throw std::invalid_argument{std::format(
        "duration must cover the complete measurement interval ending at {} seconds", measurement_end_time)};
  }

  double constexpr sprung_mass = 270;                    // kg
  double constexpr unsprung_mass = 30;                   // kg
  double constexpr suspension_spring_constant = 31000;   // N/m
  double constexpr suspension_damping_coefficient = 350; // N*s/m
  double constexpr tire_spring_constant = 196000;        // N/m
  double constexpr ground_amplitude = 0.003;             // m

  hilbertcli::state<double> state{0, 0, 0, 0, 0};

  auto constexpr ground_position_function = make_ground_position_function(ground_amplitude);
  auto constexpr state_derivative_function = make_state_derivative_function(
      sprung_mass,
      unsprung_mass,
      suspension_spring_constant,
      suspension_damping_coefficient,
      tire_spring_constant,
      ground_position_function);

  auto const steps = static_cast<size_t>(simulation.duration / simulation.time_step);
  auto const initial_platform_position = ground_position_function(state.phi());

  hilbertcli::vec_collector collector(
      steps + 1,
      {
          .time = 0,
          .xs = state.xs(),
          .xu = state.xu(),
          .ground = initial_platform_position,
          .tire_force = tire_spring_constant * (state.xu() - initial_platform_position),
      });

  for (size_t index = 0; index < steps; ++index)
  {
    auto const time = static_cast<double>(index) * simulation.time_step;
    auto const state_delta = rk4_delta(time, state, state_derivative_function, simulation.time_step);
    state = state + state_delta;

    auto const platform_position = ground_position_function(state.phi());
    auto const tire_force = tire_spring_constant * (state.xu() - platform_position);

    collector.collect({
        .time = time + simulation.time_step,
        .xs = state.xs(),
        .xu = state.xu(),
        .ground = platform_position,
        .tire_force = tire_force,
    });
  }

  auto const time = collector.time_span();
  auto const measurement_begin = std::lower_bound(time.begin(), time.end(), measurement_start_time);
  auto const measurement_end = std::lower_bound(measurement_begin, time.end(), measurement_end_time);
  auto const measurement_offset = static_cast<size_t>(measurement_begin - time.begin());
  auto const measurement_size = static_cast<size_t>(measurement_end - measurement_begin);

  if (measurement_size < 2)
  {
    throw std::invalid_argument{"measurement interval must contain at least two samples"};
  }

  auto const measurement_time = time.subspan(measurement_offset, measurement_size);
  auto const measurement_platform = collector.ground_span().subspan(measurement_offset, measurement_size);
  auto const measurement_tire_force = collector.tire_force_span().subspan(measurement_offset, measurement_size);

  auto const sampling_rate = 1.0 / simulation.time_step;
  auto const platform_signal = hilbert::calculate_inst_signal_data(measurement_platform, sampling_rate);
  auto const tire_force_signal = hilbert::calculate_inst_signal_data(measurement_tire_force, sampling_rate);

  write_simulation_data(output, collector, measurement_time, platform_signal, tire_force_signal);
}


struct command_dispatcher
{
  void
  operator()(simulation_command const &simulation) const
  {
    if (simulation.output_path)
    {
      std::ofstream output{*simulation.output_path};
      if (!output)
      {
        throw std::runtime_error{"could not open output file: " + simulation.output_path->string()};
      }
      run_simulation(simulation, output);
    }
    else
    {
      run_simulation(simulation, std::cout);
    }
  }

  void
  operator()([[maybe_unused]] help_command const &help) const
  {
    print_usage(std::cout);
  }
};


int
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
    return 2;
  }

  return 0;
}

} // namespace


int
main(int argc, char const **argv)
{
  try
  {
    return run_cli(argc, argv);
  }
  catch (...)
  {
    return 1;
  }
}
