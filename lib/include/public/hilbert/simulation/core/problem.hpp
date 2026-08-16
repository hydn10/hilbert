#ifndef HILBERT_SIMULATION_CORE_PROBLEM_HPP
#define HILBERT_SIMULATION_CORE_PROBLEM_HPP


#include <hilbert/simulation/core/concepts.hpp>
#include <hilbert/simulation/core/settings.hpp>

#include <concepts>
#include <type_traits>
#include <utility>


namespace hilbert::simulation::detail
{

struct simulation_problem_access;


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
public:
  using float_type = Float;
  using state_type = State;
  using model_type = Model;
  using integrator_type = Integrator;

  simulation_problem(simulation_settings<Float> settings, State initial_state, Model model, Integrator integrator)
      : settings_{std::move(settings)}
      , initial_state_{std::move(initial_state)}
      , model_{std::move(model)}
      , integrator_{std::move(integrator)}
  {
  }

private:
  simulation_settings<Float> settings_;
  State initial_state_;
  [[no_unique_address]]
  Model model_;
  [[no_unique_address]]
  Integrator integrator_;

  friend struct detail::simulation_problem_access;
};


template<std::floating_point Float, typename State, typename Model, typename Integrator>
struct detail::simulation_problem_traits<simulation_problem<Float, State, Model, Integrator>> : std::true_type
{
};


template<typename Simulation>
concept simulation_problem_for = detail::simulation_problem_traits<std::remove_cvref_t<Simulation>>::value;

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_CORE_PROBLEM_HPP
