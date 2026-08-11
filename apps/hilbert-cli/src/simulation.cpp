#include <hilbertcli/simulation.hpp>

#include <hilbertcli/numerics/rk4.hpp>
#include <hilbertcli/simulation/collector.hpp>
#include <hilbertcli/simulation/suspension.hpp>

#include <hilbert/hilbert.hpp>

#include <algorithm>
#include <cstddef>
#include <format>
#include <stdexcept>
#include <utility>


namespace hilbertcli
{

simulation_result
run_simulation(simulation_config const &config)
{
  if (config.duration < measurement_end_time)
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

  state<double> current_state{0, 0, 0, 0, 0};

  auto constexpr ground_position_function = make_ground_position_function(ground_amplitude);
  auto constexpr state_derivative_function = make_state_derivative_function(
      sprung_mass,
      unsprung_mass,
      suspension_spring_constant,
      suspension_damping_coefficient,
      tire_spring_constant,
      ground_position_function);

  auto const steps = static_cast<size_t>(config.duration / config.time_step);
  auto const initial_platform_position = ground_position_function(current_state.phi());

  vec_collector collector(
      steps + 1,
      {
          .time = 0,
          .xs = current_state.xs(),
          .xu = current_state.xu(),
          .ground = initial_platform_position,
          .tire_force = tire_spring_constant * (current_state.xu() - initial_platform_position),
      });

  for (size_t index = 0; index < steps; ++index)
  {
    auto const time = static_cast<double>(index) * config.time_step;
    auto const state_delta = numerics::rk4_delta(time, current_state, state_derivative_function, config.time_step);
    current_state = current_state + state_delta;

    auto const platform_position = ground_position_function(current_state.phi());
    auto const tire_force = tire_spring_constant * (current_state.xu() - platform_position);

    collector.collect({
        .time = time + config.time_step,
        .xs = current_state.xs(),
        .xu = current_state.xu(),
        .ground = platform_position,
        .tire_force = tire_force,
    });
  }

  auto const time = collector.time_span();
  auto const measurement_begin = std::ranges::lower_bound(time, measurement_start_time);
  auto const measurement_end = std::ranges::lower_bound(measurement_begin, time.end(), measurement_end_time);
  auto const measurement_offset = static_cast<size_t>(measurement_begin - time.begin());
  auto const measurement_size = static_cast<size_t>(measurement_end - measurement_begin);

  if (measurement_size < 2)
  {
    throw std::invalid_argument{"measurement interval must contain at least two samples"};
  }

  auto const measurement_platform = collector.ground_span().subspan(measurement_offset, measurement_size);
  auto const measurement_tire_force = collector.tire_force_span().subspan(measurement_offset, measurement_size);

  auto const sampling_rate = 1.0 / config.time_step;
  auto platform_signal = hilbert::calculate_inst_signal_data(measurement_platform, sampling_rate);
  auto tire_force_signal = hilbert::calculate_inst_signal_data(measurement_tire_force, sampling_rate);

  return {
      .samples = std::move(collector),
      .measurement_offset = measurement_offset,
      .measurement_size = measurement_size,
      .platform_signal = std::move(platform_signal),
      .tire_force_signal = std::move(tire_force_signal),
  };
}

} // namespace hilbertcli
