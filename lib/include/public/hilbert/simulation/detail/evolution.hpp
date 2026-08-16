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
#include <type_traits>
#include <utility>


namespace hilbert::simulation::detail
{

template<typename Model, typename Float, typename State>
concept physical_model_for =
    dynamics_for<Model, Float, State> &&
    requires(Model const &model, Float time, State const &state) { model.observe(time, state); };


template<typename Model, typename Float, typename State>
using model_delta_t = dynamics_delta_t<Model, Float, State>;


template<typename Model, typename Float, typename State>
using model_sample_t = std::remove_cvref_t<decltype(std::declval<Model const &>().observe(
    std::declval<Float>(), std::declval<State const &>()))>;


template<typename Integrator, typename Float, typename State, typename Dynamics>
concept integrator_for =
    dynamics_for<Dynamics, Float, State> &&
    requires(Integrator &integrator, Float time, State const &state, Dynamics const &dynamics, Float time_step) {
      { integrator(time, state, dynamics, time_step) } -> std::same_as<dynamics_delta_t<Dynamics, Float, State>>;
    };


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model>
class simulation_engine
{
  using delta_type = dynamics_delta_t<Model, Float, State>;
  using sample_type = model_sample_t<Model, Float, State>;

  simulation_settings<Float> settings_;
  State state_;
  [[no_unique_address]]
  Model model_;
  [[no_unique_address]]
  Integrator integrator_;
  std::size_t step_index_ = 0;
  std::size_t sample_count_;

  [[nodiscard]]
  Float
  current_time() const noexcept
  {
    return static_cast<Float>(step_index_) * settings_.time_step;
  }

public:
  simulation_engine(simulation_settings<Float> settings, State initial_state, Model model, Integrator integrator)
      : settings_{settings}
      , state_{std::move(initial_state)}
      , model_{std::move(model)}
      , integrator_{std::move(integrator)}
      , sample_count_{sample_count_for(settings)}
  {
  }

  [[nodiscard]]
  std::size_t
  sample_count() const noexcept
  {
    return sample_count_;
  }

  [[nodiscard]]
  sample_type
  current_sample() const
  {
    return model_.observe(current_time(), state_);
  }

  void
  advance()
  {
    auto const delta = integrator_(current_time(), state_, model_, settings_.time_step);
    state_ = state_ + delta;
    ++step_index_;
  }

private:
  static std::size_t
  sample_count_for(simulation_settings<Float> settings)
  {
    if (!std::isfinite(settings.time_step) || settings.time_step <= 0)
    {
      throw std::invalid_argument{"time step must be finite and positive"};
    }
    if (!std::isfinite(settings.duration) || settings.duration < 0)
    {
      throw std::invalid_argument{"duration must be finite and non-negative"};
    }

    auto const step_count_value = settings.duration / settings.time_step;
    if (step_count_value >= static_cast<Float>(std::numeric_limits<std::size_t>::max()))
    {
      throw std::invalid_argument{"simulation sample count exceeds size_t"};
    }

    return static_cast<std::size_t>(step_count_value) + 1uz;
  }
};


template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
class suspension_model
{
  suspension_parameters<Float> parameters_;
  [[no_unique_address]]
  FrequencyProfile frequency_profile_;

public:
  suspension_model(suspension_parameters<Float> parameters, FrequencyProfile frequency_profile)
      : parameters_{parameters}
      , frequency_profile_{std::move(frequency_profile)}
  {
  }

  suspension_derivative<Float>
  derivative(Float time, suspension_state<Float> const &state) const
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

  sample<Float>
  observe(Float time, suspension_state<Float> const &state) const
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
};


template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
auto
make_suspension_engine(
    simulation_settings<Float> settings,
    suspension_parameters<Float> parameters,
    FrequencyProfile frequency_profile,
    suspension_state<Float> initial_state)
{
  using state_type = suspension_state<Float>;
  using model_type = suspension_model<Float, FrequencyProfile>;
  using integrator_type = rk4<Float, state_type, model_type>;
  using engine_type = simulation_engine<Float, state_type, model_type, integrator_type>;

  return engine_type{
      std::move(settings),
      std::move(initial_state),
      model_type{std::move(parameters), std::move(frequency_profile)},
      integrator_type{},
  };
}

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_EVOLUTION_HPP
