#ifndef HILBERT_SIMULATION_DETAIL_ENGINE_HPP
#define HILBERT_SIMULATION_DETAIL_ENGINE_HPP


#include <hilbert/detail/attributes.hpp>
#include <hilbert/simulation/core/problem.hpp>

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>


namespace hilbert::simulation::detail
{

template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
class simulation_engine
{
  using sample_type = model_sample_t<Model, Float, State>;

  Float time_step_;
  State state_;
  HILBERT_NO_UNIQUE_ADDRESS
  Model model_;
  HILBERT_NO_UNIQUE_ADDRESS
  Integrator integrator_;
  std::size_t step_index_ = 0;
  std::size_t sample_count_;

  [[nodiscard]]
  Float
  current_time() const noexcept;

public:
  explicit simulation_engine(simulation_problem<Float, State, Model, Integrator> &&problem);

  [[nodiscard]]
  std::size_t
  sample_count() const noexcept;

  [[nodiscard]]
  sample_type
  current_sample() const;

  void
  advance();
};


template<typename Simulation>
using engine_for_t = simulation_engine<
    typename std::remove_cvref_t<Simulation>::float_type,
    typename std::remove_cvref_t<Simulation>::state_type,
    typename std::remove_cvref_t<Simulation>::model_type,
    typename std::remove_cvref_t<Simulation>::integrator_type>;


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
Float
simulation_engine<Float, State, Model, Integrator>::current_time() const noexcept
{
  return static_cast<Float>(step_index_) * time_step_;
}


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
             executable_state<State>
simulation_engine<Float, State, Model, Integrator>::simulation_engine(
    simulation_problem<Float, State, Model, Integrator> &&problem)
    : time_step_{problem.time_step_}
    , state_{std::move(problem.initial_state_)}
    , model_{std::move(problem.model_)}
    , integrator_{std::move(problem.integrator_)}
    , sample_count_{problem.sample_count_}
{
}


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
std::size_t
simulation_engine<Float, State, Model, Integrator>::sample_count() const noexcept
{
  return sample_count_;
}


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
simulation_engine<Float, State, Model, Integrator>::sample_type
simulation_engine<Float, State, Model, Integrator>::current_sample() const
{
  return model_.observe(current_time(), state_);
}


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
void
simulation_engine<Float, State, Model, Integrator>::advance()
{
  auto const delta = integrator_(current_time(), state_, model_, time_step_);
  state_ = state_ + delta;
  ++step_index_;
}


} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_ENGINE_HPP
