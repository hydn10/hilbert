#ifndef HILBERT_SIMULATION_CORE_CONCEPTS_HPP
#define HILBERT_SIMULATION_CORE_CONCEPTS_HPP


#include <concepts>
#include <type_traits>
#include <utility>


namespace hilbert::simulation
{

template<typename State, typename Delta, typename Scalar>
concept state_delta_algebra =
    std::floating_point<Scalar> && requires(State const &state, Delta const &left, Delta const &right, Scalar scalar) {
      { state + left } -> std::same_as<State>;
      { left + right } -> std::same_as<Delta>;
      { left * scalar } -> std::same_as<Delta>;
    };


template<typename State>
concept executable_state = requires(State &current, State next) {
  { current = std::move(next) } -> std::same_as<State &>;
};


template<typename Dynamics, typename Float, typename State>
using dynamics_delta_t = std::remove_cvref_t<decltype(std::declval<Dynamics const &>().derivative(
    std::declval<Float>(), std::declval<State const &>()))>;


template<typename Dynamics, typename Float, typename State>
concept dynamics_for =
    std::floating_point<Float> && requires(Dynamics const &dynamics, Float time, State const &state) {
      { dynamics.derivative(time, state) } -> std::same_as<dynamics_delta_t<Dynamics, Float, State>>;
    } && state_delta_algebra<State, dynamics_delta_t<Dynamics, Float, State>, Float>;


template<typename Model, typename Float, typename State>
using model_sample_t = std::remove_cvref_t<decltype(std::declval<Model const &>().observe(
    std::declval<Float>(), std::declval<State const &>()))>;


template<typename Model, typename Float, typename State>
concept physical_model_for =
    dynamics_for<Model, Float, State> && requires(Model const &model, Float time, State const &state) {
      { model.observe(time, state) } -> std::same_as<model_sample_t<Model, Float, State>>;
    };


template<typename Integrator, typename Float, typename State, typename Dynamics>
concept integrator_for =
    dynamics_for<Dynamics, Float, State> &&
    requires(Integrator &integrator, Float time, State const &state, Dynamics const &dynamics, Float time_step) {
      { integrator(time, state, dynamics, time_step) } -> std::same_as<dynamics_delta_t<Dynamics, Float, State>>;
    };

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_CORE_CONCEPTS_HPP
