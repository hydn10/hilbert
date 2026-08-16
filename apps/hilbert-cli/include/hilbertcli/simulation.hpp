#ifndef HILBERTCLI_SIMULATION_HPP
#define HILBERTCLI_SIMULATION_HPP


#include <hilbert/hilbert.hpp>
#include <hilbert/simulation/config.hpp>
#include <hilbert/simulation/frequency/scheduled.hpp>
#include <hilbert/simulation/sinks/soa_vector.hpp>

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
using frequency_profile = hilbert::simulation::frequency::scheduled<Float>;

template<std::floating_point Float>
inline constexpr Float measurement_start_time = frequency_profile<Float>::measurement::start_time;

template<std::floating_point Float>
inline constexpr Float measurement_end_time = frequency_profile<Float>::measurement::end_time;

template<std::floating_point Float>
inline constexpr Float hilbert_margin_time = static_cast<Float>(0.2);

template<std::floating_point Float>
inline constexpr Float hilbert_start_time = measurement_start_time<Float> - hilbert_margin_time<Float>;

template<std::floating_point Float>
inline constexpr Float hilbert_end_time = measurement_end_time<Float> + hilbert_margin_time<Float>;


template<std::floating_point Float>
struct time_interval
{
  Float start_time;
  Float end_time;
};


template<std::floating_point Float>
using simulation_data = typename hilbert::simulation::sinks::soa_vector_sink<Float>::simulation_data;


template<std::floating_point Float>
struct analysis_result
{
  size_t hilbert_offset;
  size_t hilbert_size;
  time_interval<Float> measurement_interval;
  time_interval<Float> hilbert_interval;
  hilbert::signal_data<Float> platform_signal;
  hilbert::signal_data<Float> tire_force_signal;
};


template<hilbert::supported_float Float>
analysis_result<Float>
analyze_simulation(
    hilbert::simulation::simulation_settings<Float> const &settings, simulation_data<Float> const &samples)
{
  if (settings.duration < hilbert_end_time<Float>)
  {
    throw std::invalid_argument{
        std::format("duration must cover the complete Hilbert interval ending at {} seconds", hilbert_end_time<Float>)};
  }

  auto const time = samples.time_span();
  auto const hilbert_begin = std::ranges::lower_bound(time, hilbert_start_time<Float>);
  auto const hilbert_end = std::ranges::lower_bound(hilbert_begin, time.end(), hilbert_end_time<Float>);
  auto const hilbert_offset = static_cast<size_t>(hilbert_begin - time.begin());
  auto const hilbert_size = static_cast<size_t>(hilbert_end - hilbert_begin);

  if (hilbert_size < 2uz)
  {
    throw std::invalid_argument{"Hilbert interval must contain at least two samples"};
  }

  auto const hilbert_platform = samples.ground_displacement_span().subspan(hilbert_offset, hilbert_size);
  auto const hilbert_tire_force = samples.tire_force_span().subspan(hilbert_offset, hilbert_size);
  auto const sampling_rate = static_cast<Float>(1) / settings.time_step;
  auto platform_signal = hilbert::calculate_inst_signal_data(hilbert_platform, sampling_rate);
  auto tire_force_signal = hilbert::calculate_inst_signal_data(hilbert_tire_force, sampling_rate);

  return {
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
