#ifndef HILBERT_SIMULATION_HPP
#define HILBERT_SIMULATION_HPP


#include <hilbert/simulation/config.hpp>
#include <hilbert/simulation/detail/evolution.hpp>
#include <hilbert/simulation/frequency/constant.hpp>
#include <hilbert/simulation/frequency/profile.hpp>
#include <hilbert/simulation/frequency/scheduled.hpp>
#include <hilbert/simulation/sample.hpp>
#include <hilbert/simulation/sinks/soa_vector.hpp>
#include <hilbert/simulation/sinks/vector.hpp>
#include <hilbert/simulation/view.hpp>

#include <concepts>
#include <cstddef>
#include <functional>
#include <utility>


namespace hilbert::simulation
{

template<std::floating_point Float, frequency::profile<Float> FrequencyProfile, typename SinkFactory>
decltype(auto)
run_simulation(
    simulation_settings<Float> settings,
    suspension_parameters<Float> parameters,
    FrequencyProfile frequency_profile,
    SinkFactory &&sink_factory)
{
  auto engine = detail::make_suspension_engine(
      std::move(settings),
      std::move(parameters),
      std::move(frequency_profile),
      detail::suspension_state<Float>{0, 0, 0, 0, 0});
  auto const count = engine.sample_count();
  auto sink = std::invoke(std::forward<SinkFactory>(sink_factory), count);

  sink.push(engine.current_sample());
  for (auto index = 1uz; index < count; ++index)
  {
    engine.advance();
    sink.push(engine.current_sample());
  }

  return std::move(sink).finish();
}

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_HPP
