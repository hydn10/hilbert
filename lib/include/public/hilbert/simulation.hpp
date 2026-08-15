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

#include <concepts>
#include <cstddef>
#include <functional>
#include <utility>


namespace hilbert::simulation
{

template<std::floating_point Float, frequency::profile<Float> FrequencyProfile, typename SinkFactory>
decltype(auto)
run_simulation(config<Float> const &simulation_config, FrequencyProfile frequency_profile, SinkFactory &&sink_factory)
{
  auto initialized = detail::initialize_simulation(
      simulation_config, std::move(frequency_profile), detail::suspension_state<Float>{0, 0, 0, 0, 0});
  auto sink = std::invoke(std::forward<SinkFactory>(sink_factory), initialized.sample_count);

  sink.push(detail::observe_simulation(initialized.state));
  for (auto index = 1uz; index < initialized.sample_count; ++index)
  {
    detail::advance_simulation(initialized.state);
    sink.push(detail::observe_simulation(initialized.state));
  }

  return std::move(sink).finish();
}

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_HPP
