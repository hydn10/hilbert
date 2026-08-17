#ifndef HILBERT_SIMULATION_CORE_PROBLEM_HPP
#define HILBERT_SIMULATION_CORE_PROBLEM_HPP


#include <hilbert/detail/attributes.hpp>
#include <hilbert/simulation/core/concepts.hpp>
#include <hilbert/simulation/core/settings.hpp>

#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>


namespace hilbert::simulation::detail
{

template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
class simulation_engine;


template<typename Simulation>
struct simulation_problem_traits : std::false_type
{
};

} // namespace hilbert::simulation::detail


namespace hilbert::simulation
{

template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
class simulation_problem
{
  Float time_step_;
  std::size_t sample_count_;
  State initial_state_;
  HILBERT_NO_UNIQUE_ADDRESS
  Model model_;
  HILBERT_NO_UNIQUE_ADDRESS
  Integrator integrator_;

  static std::size_t
  sample_count_for(simulation_settings<Float> settings);

  friend class detail::simulation_engine<Float, State, Model, Integrator>;

public:
  using float_type = Float;
  using state_type = State;
  using model_type = Model;
  using integrator_type = Integrator;

  simulation_problem(simulation_settings<Float> settings, State initial_state, Model model, Integrator integrator);
};


template<std::floating_point Float, typename State, typename Model, typename Integrator>
struct detail::simulation_problem_traits<simulation_problem<Float, State, Model, Integrator>> : std::true_type
{
};


template<typename Simulation>
concept simulation_problem_for = detail::simulation_problem_traits<std::remove_cvref_t<Simulation>>::value;


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
             executable_state<State>
simulation_problem<Float, State, Model, Integrator>::simulation_problem(
    simulation_settings<Float> settings, State initial_state, Model model, Integrator integrator)
    : time_step_{settings.time_step}
    , sample_count_{sample_count_for(settings)}
    , initial_state_{std::move(initial_state)}
    , model_{std::move(model)}
    , integrator_{std::move(integrator)}
{
}


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
std::size_t
simulation_problem<Float, State, Model, Integrator>::sample_count_for(simulation_settings<Float> settings)
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

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_CORE_PROBLEM_HPP
