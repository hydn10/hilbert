#ifndef HILBERTCLI_SIMULATION_HPP
#define HILBERTCLI_SIMULATION_HPP


#include <hilbertcli/numerics/rk4.hpp>
#include <hilbertcli/simulation/collector.hpp>
#include <hilbertcli/simulation/parameters.hpp>
#include <hilbertcli/simulation/suspension.hpp>

#include <hilbert/hilbert.hpp>

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <format>
#include <ranges>
#include <stdexcept>
#include <utility>


namespace hilbertcli
{

template<std::floating_point Float>
struct simulation_config
{
  Float time_step = static_cast<Float>(0.0005);
  Float duration = static_cast<Float>(20.0);
  simulation_parameters<Float> parameters;
};


template<std::floating_point Float>
struct time_interval
{
  Float start_time;
  Float end_time;
};


template<std::floating_point Float>
struct simulation_result
{
  vec_collector<Float> samples;
  size_t hilbert_offset;
  size_t hilbert_size;
  time_interval<Float> measurement_interval;
  time_interval<Float> hilbert_interval;
  hilbert::signal_data<Float> platform_signal;
  hilbert::signal_data<Float> tire_force_signal;
};


template<hilbert::supported_float Float, ground_frequency_profile<Float> FrequencyProfile>
simulation_result<Float>
run_simulation(simulation_config<Float> const &config, FrequencyProfile frequency_profile)
{
  if (config.duration < hilbert_end_time<Float>)
  {
    throw std::invalid_argument{
        std::format("duration must cover the complete Hilbert interval ending at {} seconds", hilbert_end_time<Float>)};
  }

  state<Float> current_state{0, 0, 0, 0, 0};

  auto const ground_position_function = make_ground_position_function(config.parameters.ground_amplitude);
  auto const state_derivative_function =
      make_state_derivative_function<Float>(config.parameters, ground_position_function, frequency_profile);

  auto const steps = static_cast<size_t>(config.duration / config.time_step);
  auto const initial_platform_position = ground_position_function(current_state.phi());

  vec_collector<Float> collector(
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
    auto const time = static_cast<Float>(index) * config.time_step;
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
  auto const hilbert_begin = std::ranges::lower_bound(time, hilbert_start_time<Float>);
  auto const hilbert_end = std::ranges::lower_bound(hilbert_begin, time.end(), hilbert_end_time<Float>);
  auto const hilbert_offset = static_cast<size_t>(hilbert_begin - time.begin());
  auto const hilbert_size = static_cast<size_t>(hilbert_end - hilbert_begin);

  if (hilbert_size < 2uz)
  {
    throw std::invalid_argument{"Hilbert interval must contain at least two samples"};
  }

  auto const hilbert_platform = collector.ground_span().subspan(hilbert_offset, hilbert_size);
  auto const hilbert_tire_force = collector.tire_force_span().subspan(hilbert_offset, hilbert_size);

  auto const sampling_rate = static_cast<Float>(1) / config.time_step;
  auto platform_signal = hilbert::calculate_inst_signal_data(hilbert_platform, sampling_rate);
  auto tire_force_signal = hilbert::calculate_inst_signal_data(hilbert_tire_force, sampling_rate);

  return {
      .samples = std::move(collector),
      .hilbert_offset = hilbert_offset,
      .hilbert_size = hilbert_size,
      .measurement_interval = {.start_time = measurement_start_time<Float>, .end_time = measurement_end_time<Float>},
      .hilbert_interval = {.start_time = hilbert_start_time<Float>, .end_time = hilbert_end_time<Float>},
      .platform_signal = std::move(platform_signal),
      .tire_force_signal = std::move(tire_force_signal),
  };
}

} // namespace hilbertcli

#endif // HILBERTCLI_SIMULATION_HPP
