#ifndef HILBERT_SIMULATION_DETAIL_RK4_HPP
#define HILBERT_SIMULATION_DETAIL_RK4_HPP


#include <concepts>
#include <type_traits>
#include <utility>


namespace hilbert::simulation::detail
{

template<typename State, typename Delta, typename Scalar>
concept state_delta_algebra =
    std::floating_point<Scalar> && requires(State const &state, Delta const &left, Delta const &right, Scalar scalar) {
      { state + left } -> std::same_as<State>;
      { left + right } -> std::same_as<Delta>;
      { left * scalar } -> std::same_as<Delta>;
    };


template<typename Dynamics, typename Float, typename State>
using dynamics_delta_t = std::remove_cvref_t<decltype(std::declval<Dynamics const &>().derivative(
    std::declval<Float>(), std::declval<State const &>()))>;


template<typename Dynamics, typename Float, typename State>
concept dynamics_for =
    std::floating_point<Float> && requires(Dynamics const &dynamics, Float time, State const &state) {
      dynamics.derivative(time, state);
    } && state_delta_algebra<State, dynamics_delta_t<Dynamics, Float, State>, Float>;


template<std::floating_point Float, typename State, typename Dynamics>
requires dynamics_for<Dynamics, Float, State>
class rk4
{
public:
  using delta_type = dynamics_delta_t<Dynamics, Float, State>;

  delta_type
  operator()(Float time, State const &state, Dynamics const &dynamics, Float time_step) const
  {
    Float constexpr two = static_cast<Float>(2);
    Float constexpr six = static_cast<Float>(6);

    auto const k1 = dynamics.derivative(time, state);
    auto const k2 = dynamics.derivative(time + time_step / two, state + k1 * (time_step / two));
    auto const k3 = dynamics.derivative(time + time_step / two, state + k2 * (time_step / two));
    auto const k4 = dynamics.derivative(time + time_step, state + k3 * time_step);

    return (k1 + k2 * two + k3 * two + k4) * (time_step / six);
  }
};

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_RK4_HPP
