#ifndef HILBERT_SIMULATION_SUSPENSION_FACTORY_HPP
#define HILBERT_SIMULATION_SUSPENSION_FACTORY_HPP


#include <hilbert/simulation/core/problem.hpp>
#include <hilbert/simulation/integrators/rk4.hpp>
#include <hilbert/simulation/suspension/model.hpp>

#include <concepts>
#include <utility>


namespace hilbert::simulation::suspension
{

template<std::floating_point Float, ground_frequency::profile<Float> FrequencyProfile>
auto
make_simulation(
    simulation_settings<Float> settings,
    parameters<Float> parameters,
    FrequencyProfile frequency_profile,
    state<Float> initial_state)
{
  using state_type = state<Float>;
  using model_type = model<Float, FrequencyProfile>;
  using integrator_type = integrators::rk4<Float, state_type, model_type>;

  return simulation_problem<Float, state_type, model_type, integrator_type>{
      std::move(settings),
      std::move(initial_state),
      model_type{std::move(parameters), std::move(frequency_profile)},
      integrator_type{},
  };
}

} // namespace hilbert::simulation::suspension

#endif // HILBERT_SIMULATION_SUSPENSION_FACTORY_HPP
