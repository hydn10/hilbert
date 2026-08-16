#ifndef HILBERT_SIMULATION_DRIVERS_RUN_HPP
#define HILBERT_SIMULATION_DRIVERS_RUN_HPP


#include <hilbert/simulation/core/problem.hpp>
#include <hilbert/simulation/detail/engine.hpp>

#include <functional>
#include <utility>


namespace hilbert::simulation
{

template<typename Simulation, typename SinkFactory>
requires simulation_problem_for<Simulation>
decltype(auto)
run_simulation(Simulation simulation, SinkFactory &&sink_factory)
{
  auto engine = detail::make_engine(std::move(simulation));
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

#endif // HILBERT_SIMULATION_DRIVERS_RUN_HPP
