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
    std::floating_point<Float> && requires(Model const &model, Float time, State const &state) {
      model.derivative(time, state);
      model.observe(time, state);
    };


template<typename Model, typename Float, typename State>
using model_delta_t = std::remove_cvref_t<decltype(std::declval<Model const &>().derivative(
    std::declval<Float>(), std::declval<State const &>()))>;


template<typename Model, typename Float, typename State>
using model_sample_t = std::remove_cvref_t<decltype(std::declval<Model const &>().observe(
    std::declval<Float>(), std::declval<State const &>()))>;


template<typename Float, typename State, typename Delta>
struct derivative_placeholder
{
  Delta
  operator()(Float, State const &) const;
};


template<typename Integrator, typename Float, typename State, typename Delta>
concept integrator_for = std::floating_point<Float> && requires(
                                                           Integrator &integrator,
                                                           Float time,
                                                           State const &state,
                                                           Float time_step,
                                                           derivative_placeholder<Float, State, Delta> derivative) {
  { integrator(time, state, derivative, time_step) } -> std::same_as<Delta>;
};


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> &&
         state_delta_algebra<State, model_delta_t<Model, Float, State>, Float> &&
         integrator_for<Integrator, Float, State, model_delta_t<Model, Float, State>>
class simulation_engine
{
  using delta_type = model_delta_t<Model, Float, State>;
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
    auto const time = current_time();
    auto derivative = [this](Float derivative_time, State const &current_state) -> delta_type
    {
      return model_.derivative(derivative_time, current_state);
    };
    auto const delta = integrator_(time, state_, derivative, settings_.time_step);
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
make_suspension_engine(config<Float> const &simulation_config, FrequencyProfile frequency_profile)
{
  using state_type = suspension_state<Float>;
  using model_type = suspension_model<Float, FrequencyProfile>;
  using engine_type = simulation_engine<Float, state_type, model_type, rk4>;

  return engine_type{
      simulation_settings<Float>{simulation_config.time_step, simulation_config.duration},
      state_type{0, 0, 0, 0, 0},
      model_type{simulation_config.parameters, std::move(frequency_profile)},
      rk4{},
  };
}

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_EVOLUTION_HPP
