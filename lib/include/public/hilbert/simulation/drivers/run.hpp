#ifndef HILBERT_SIMULATION_DRIVERS_RUN_HPP
#define HILBERT_SIMULATION_DRIVERS_RUN_HPP


#include <hilbert/simulation/core/input_count.hpp>
#include <hilbert/simulation/core/problem.hpp>
#include <hilbert/simulation/detail/simulation_engine.hpp>
#include <hilbert/simulation/sinks/concepts.hpp>

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>


namespace hilbert::simulation
{

template<typename SinkFactory, typename Simulation>
concept sink_factory_for_simulation =
    simulation_problem_for<Simulation> && sinks::sink_factory_for<
                                              SinkFactory,
                                              exact_input_count,
                                              model_sample_t<
                                                  typename std::remove_cvref_t<Simulation>::model_type,
                                                  typename std::remove_cvref_t<Simulation>::float_type,
                                                  typename std::remove_cvref_t<Simulation>::state_type>>;


template<simulation_problem_for Simulation, sink_factory_for_simulation<Simulation> SinkFactory>
decltype(auto)
run_simulation(Simulation simulation, SinkFactory &&sink_factory);


template<simulation_problem_for Simulation, sink_factory_for_simulation<Simulation> SinkFactory>
decltype(auto)
run_simulation(Simulation simulation, SinkFactory &&sink_factory)
{
  using engine_type = detail::engine_for_t<Simulation>;

  auto engine = engine_type{std::move(simulation)};
  auto const count = exact_input_count{engine.sample_count()};
  auto sink = std::invoke(std::forward<SinkFactory>(sink_factory), count);

  sink.push(engine.current_sample());
  for (auto index = 1uz; index < count.value(); ++index)
  {
    engine.advance();
    sink.push(engine.current_sample());
  }

  return std::move(sink).finish();
}

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_DRIVERS_RUN_HPP
