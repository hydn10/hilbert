#ifndef HILBERT_SIMULATION_SUSPENSION_MODEL_HPP
#define HILBERT_SIMULATION_SUSPENSION_MODEL_HPP


#include <hilbert/simulation/suspension/frequency.hpp>
#include <hilbert/simulation/suspension/parameters.hpp>
#include <hilbert/simulation/suspension/sample.hpp>
#include <hilbert/simulation/suspension/state.hpp>

#include <cmath>
#include <concepts>
#include <numbers>
#include <utility>


namespace hilbert::simulation::suspension
{

template<std::floating_point Float, ground_frequency::profile<Float> FrequencyProfile>
class model
{
  parameters<Float> parameters_;
  [[no_unique_address]]
  FrequencyProfile frequency_profile_;

public:
  model(parameters<Float> parameters, FrequencyProfile frequency_profile);

  state_derivative<Float>
  derivative(Float time, state<Float> const &state) const;

  sample<Float>
  observe(Float time, state<Float> const &state) const;
};


template<std::floating_point Float, ground_frequency::profile<Float> FrequencyProfile>
model<Float, FrequencyProfile>::model(parameters<Float> parameters, FrequencyProfile frequency_profile)
    : parameters_{std::move(parameters)}
    , frequency_profile_{std::move(frequency_profile)}
{
}


template<std::floating_point Float, ground_frequency::profile<Float> FrequencyProfile>
state_derivative<Float>
model<Float, FrequencyProfile>::derivative(Float time, state<Float> const &state) const
{
  Float const phase_velocity = 2 * std::numbers::pi_v<Float> * static_cast<Float>(frequency_profile_(time));
  Float const ground_displacement = parameters_.ground_amplitude * std::sin(state.phase());
  Float const sprung_acceleration =
      (-parameters_.suspension_damping_coefficient * (state.sprung_velocity() - state.unsprung_velocity()) -
       parameters_.suspension_spring_constant * (state.sprung_displacement() - state.unsprung_displacement())) /
      parameters_.sprung_mass;
  Float const unsprung_acceleration =
      (parameters_.suspension_damping_coefficient * (state.sprung_velocity() - state.unsprung_velocity()) +
       parameters_.suspension_spring_constant * (state.sprung_displacement() - state.unsprung_displacement()) -
       parameters_.tire_spring_constant * (state.unsprung_displacement() - ground_displacement)) /
      parameters_.unsprung_mass;

  return {
      phase_velocity,
      state.sprung_velocity(),
      state.unsprung_velocity(),
      sprung_acceleration,
      unsprung_acceleration,
  };
}


template<std::floating_point Float, ground_frequency::profile<Float> FrequencyProfile>
sample<Float>
model<Float, FrequencyProfile>::observe(Float time, state<Float> const &state) const
{
  auto const ground_displacement = parameters_.ground_amplitude * std::sin(state.phase());

  return {
      .time = time,
      .sprung_displacement = state.sprung_displacement(),
      .unsprung_displacement = state.unsprung_displacement(),
      .ground_displacement = ground_displacement,
      .tire_force = parameters_.tire_spring_constant * (state.unsprung_displacement() - ground_displacement),
  };
}

} // namespace hilbert::simulation::suspension

#endif // HILBERT_SIMULATION_SUSPENSION_MODEL_HPP
