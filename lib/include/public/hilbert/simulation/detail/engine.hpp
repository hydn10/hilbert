#ifndef HILBERT_SIMULATION_DETAIL_ENGINE_HPP
#define HILBERT_SIMULATION_DETAIL_ENGINE_HPP


#include <hilbert/simulation/core/problem.hpp>

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
class simulation_engine
{
  using sample_type = model_sample_t<Model, Float, State>;

  Float time_step_;
  State state_;
  [[no_unique_address]]
  Model model_;
  [[no_unique_address]]
  Integrator integrator_;
  std::size_t step_index_ = 0;
  std::size_t sample_count_;

  [[nodiscard]]
  Float
  current_time() const noexcept;

public:
  simulation_engine(simulation_settings<Float> settings, State initial_state, Model model, Integrator integrator);

  [[nodiscard]]
  std::size_t
  sample_count() const noexcept;

  [[nodiscard]]
  sample_type
  current_sample() const;

  void
  advance();

private:
  static std::size_t
  sample_count_for(simulation_settings<Float> settings);
};


template<typename Simulation>
using engine_for_t = simulation_engine<
    typename std::remove_cvref_t<Simulation>::float_type,
    typename std::remove_cvref_t<Simulation>::state_type,
    typename std::remove_cvref_t<Simulation>::model_type,
    typename std::remove_cvref_t<Simulation>::integrator_type>;


struct simulation_problem_access
{
  template<simulation_problem_for Simulation>
  static engine_for_t<Simulation>
  make_engine(Simulation simulation);
};


template<simulation_problem_for Simulation>
engine_for_t<Simulation>
make_engine(Simulation simulation);


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
    simulation_settings<Float> settings, State initial_state, Model model, Integrator integrator)
    : time_step_{settings.time_step}
    , state_{std::move(initial_state)}
    , model_{std::move(model)}
    , integrator_{std::move(integrator)}
    , sample_count_{sample_count_for(settings)}
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
typename simulation_engine<Float, State, Model, Integrator>::sample_type
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


template<std::floating_point Float, typename State, typename Model, typename Integrator>
requires physical_model_for<Model, Float, State> && integrator_for<Integrator, Float, State, Model> &&
         executable_state<State>
std::size_t
simulation_engine<Float, State, Model, Integrator>::sample_count_for(simulation_settings<Float> settings)
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


template<simulation_problem_for Simulation>
engine_for_t<Simulation>
simulation_problem_access::make_engine(Simulation simulation)
{
  using engine_type = engine_for_t<Simulation>;

  return engine_type{
      std::move(simulation.settings_),
      std::move(simulation.initial_state_),
      std::move(simulation.model_),
      std::move(simulation.integrator_),
  };
}


template<simulation_problem_for Simulation>
engine_for_t<Simulation>
make_engine(Simulation simulation)
{
  return simulation_problem_access::make_engine(std::move(simulation));
}

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_ENGINE_HPP
