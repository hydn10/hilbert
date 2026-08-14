#ifndef HILBERTCLI_SIMULATION_HPP
#define HILBERTCLI_SIMULATION_HPP


#include <hilbertcli/numerics/rk4.hpp>
#include <hilbertcli/simulation/collector.hpp>
#include <hilbertcli/simulation/parameters.hpp>
#include <hilbertcli/simulation/suspension.hpp>

#include <hilbert/hilbert.hpp>

#include <algorithm>
#include <cstddef>
#include <format>
#include <ranges>
#include <stdexcept>
#include <utility>


namespace hilbertcli
{

struct simulation_config
{
  double time_step = 0.0005;
  double duration = 20.0;
  simulation_parameters parameters;
};


struct time_interval
{
  double start_time;
  double end_time;
};


struct simulation_result
{
  vec_collector samples;
  size_t hilbert_offset;
  size_t hilbert_size;
  time_interval measurement_interval;
  time_interval hilbert_interval;
  hilbert::signal_data<double> platform_signal;
  hilbert::signal_data<double> tire_force_signal;
};


template<std::floating_point Float, ground_frequency_profile<Float> FrequencyProfile>
simulation_result
run_simulation(simulation_config const &config, FrequencyProfile frequency_profile)
{
  if (config.duration < hilbert_end_time)
  {
    throw std::invalid_argument{
        std::format("duration must cover the complete Hilbert interval ending at {} seconds", hilbert_end_time)};
  }

  state<double> current_state{0, 0, 0, 0, 0};

  auto const ground_position_function = make_ground_position_function(config.parameters.ground_amplitude);
  auto const state_derivative_function =
      make_state_derivative_function<double>(config.parameters, ground_position_function, frequency_profile);

  auto const steps = static_cast<size_t>(config.duration / config.time_step);
  auto const initial_platform_position = ground_position_function(current_state.phi());

  vec_collector collector(
      steps + 1uz,
      {
          .time = 0,
          .xs = current_state.xs(),
          .xu = current_state.xu(),
          .ground = initial_platform_position,
          .tire_force = config.parameters.tire_spring_constant * (current_state.xu() - initial_platform_position),
      });

  for (auto index = 0uz; index < steps; ++index)
  {
    auto const time = static_cast<double>(index) * config.time_step;
    auto const state_delta = numerics::rk4_delta(time, current_state, state_derivative_function, config.time_step);
    current_state = current_state + state_delta;

    auto const platform_position = ground_position_function(current_state.phi());
    auto const tire_force = config.parameters.tire_spring_constant * (current_state.xu() - platform_position);

    collector.collect({
        .time = time + config.time_step,
        .xs = current_state.xs(),
        .xu = current_state.xu(),
        .ground = platform_position,
        .tire_force = tire_force,
    });
  }

  auto const time = collector.time_span();
  auto const hilbert_begin = std::ranges::lower_bound(time, hilbert_start_time);
  auto const hilbert_end = std::ranges::lower_bound(hilbert_begin, time.end(), hilbert_end_time);
  auto const hilbert_offset = static_cast<size_t>(hilbert_begin - time.begin());
  auto const hilbert_size = static_cast<size_t>(hilbert_end - hilbert_begin);

  if (hilbert_size < 2uz)
  {
    throw std::invalid_argument{"Hilbert interval must contain at least two samples"};
  }

  auto const hilbert_platform = collector.ground_span().subspan(hilbert_offset, hilbert_size);
  auto const hilbert_tire_force = collector.tire_force_span().subspan(hilbert_offset, hilbert_size);

  auto const sampling_rate = 1.0 / config.time_step;
  auto platform_signal = hilbert::calculate_inst_signal_data(hilbert_platform, sampling_rate);
  auto tire_force_signal = hilbert::calculate_inst_signal_data(hilbert_tire_force, sampling_rate);

  return {
      .samples = std::move(collector),
      .hilbert_offset = hilbert_offset,
      .hilbert_size = hilbert_size,
      .measurement_interval = {.start_time = measurement_start_time, .end_time = measurement_end_time},
      .hilbert_interval = {.start_time = hilbert_start_time, .end_time = hilbert_end_time},
      .platform_signal = std::move(platform_signal),
      .tire_force_signal = std::move(tire_force_signal),
  };
}

} // namespace hilbertcli

#endif // HILBERTCLI_SIMULATION_HPP
