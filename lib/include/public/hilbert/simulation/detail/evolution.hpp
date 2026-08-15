#ifndef HILBERT_SIMULATION_DETAIL_EVOLUTION_HPP
#define HILBERT_SIMULATION_DETAIL_EVOLUTION_HPP


#include <hilbert/simulation/config.hpp>
#include <hilbert/simulation/detail/rk4.hpp>
#include <hilbert/simulation/detail/suspension_state.hpp>
#include <hilbert/simulation/frequency/profile.hpp>
#include <hilbert/simulation/sample.hpp>

#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <utility>


namespace hilbert::simulation::detail
{

template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
struct simulation_state
{
  suspension_parameters<Float> simulation_parameters;
  Float time_step;
  FrequencyProfile frequency_profile;
  suspension_state<Float> suspension;
  size_t step_index = 0;
  Float sample_time = 0;
};


template<typename State>
struct initialized_simulation
{
  State state;
  size_t sample_count;
};


template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
auto
initialize_simulation(
    config<Float> const &simulation_config,
    FrequencyProfile frequency_profile,
    suspension_state<Float> initial_suspension_state)
{
  if (!std::isfinite(simulation_config.time_step) || simulation_config.time_step <= 0)
  {
    throw std::invalid_argument{"time step must be finite and positive"};
  }
  if (!std::isfinite(simulation_config.duration) || simulation_config.duration < 0)
  {
    throw std::invalid_argument{"duration must be finite and non-negative"};
  }

  auto const step_count_value = simulation_config.duration / simulation_config.time_step;
  if (step_count_value >= static_cast<Float>(std::numeric_limits<size_t>::max()))
  {
    throw std::invalid_argument{"simulation sample count exceeds size_t"};
  }

  auto const step_count = static_cast<size_t>(step_count_value);
  using state_type = simulation_state<Float, FrequencyProfile>;
  return initialized_simulation<state_type>{
      .state =
          {
              .simulation_parameters = simulation_config.parameters,
              .time_step = simulation_config.time_step,
              .frequency_profile = std::move(frequency_profile),
              .suspension = std::move(initial_suspension_state),
          },
      .sample_count = step_count + 1uz,
  };
}


template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
sample<Float>
observe_simulation(simulation_state<Float, FrequencyProfile> const &state)
{
  auto const ground_displacement = state.simulation_parameters.ground_amplitude * std::sin(state.suspension.phase());

  return {
      .time = state.sample_time,
      .sprung_displacement = state.suspension.sprung_displacement(),
      .unsprung_displacement = state.suspension.unsprung_displacement(),
      .ground_displacement = ground_displacement,
      .tire_force = state.simulation_parameters.tire_spring_constant *
                    (state.suspension.unsprung_displacement() - ground_displacement),
  };
}


template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
void
advance_simulation(simulation_state<Float, FrequencyProfile> &state)
{
  auto const derivative = [&state](Float time, suspension_state<Float> const &current_state)
  {
    auto const &parameters = state.simulation_parameters;
    Float const phase_velocity = 2 * std::numbers::pi_v<Float> * static_cast<Float>(state.frequency_profile(time));
    Float const ground_displacement = parameters.ground_amplitude * std::sin(current_state.phase());
    Float const sprung_acceleration =
        (-parameters.suspension_damping_coefficient *
             (current_state.sprung_velocity() - current_state.unsprung_velocity()) -
         parameters.suspension_spring_constant *
             (current_state.sprung_displacement() - current_state.unsprung_displacement())) /
        parameters.sprung_mass;
    Float const unsprung_acceleration =
        (parameters.suspension_damping_coefficient *
             (current_state.sprung_velocity() - current_state.unsprung_velocity()) +
         parameters.suspension_spring_constant *
             (current_state.sprung_displacement() - current_state.unsprung_displacement()) -
         parameters.tire_spring_constant * (current_state.unsprung_displacement() - ground_displacement)) /
        parameters.unsprung_mass;

    return suspension_derivative<Float>{
        phase_velocity,
        current_state.sprung_velocity(),
        current_state.unsprung_velocity(),
        sprung_acceleration,
        unsprung_acceleration,
    };
  };

  auto const integration_time = static_cast<Float>(state.step_index) * state.time_step;
  auto const delta = rk4_delta(integration_time, state.suspension, derivative, state.time_step);
  state.suspension = state.suspension + delta;
  state.sample_time = integration_time + state.time_step;
  ++state.step_index;
}

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_EVOLUTION_HPP
