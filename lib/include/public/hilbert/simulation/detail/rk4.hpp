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


template<typename Float, typename State, typename DerivativeFunction>
concept derivative_function_for =
    std::floating_point<Float> &&
    requires(DerivativeFunction &derivative, Float time, State const &state) { derivative(time, state); };


template<typename Float, typename State, typename DerivativeFunction>
using derivative_result_t = std::remove_cvref_t<decltype(std::declval<DerivativeFunction &>()(
    std::declval<Float>(), std::declval<State const &>()))>;


struct rk4
{
  template<std::floating_point Float, typename State, typename DerivativeFunction>
  requires derivative_function_for<Float, State, DerivativeFunction> &&
           state_delta_algebra<State, derivative_result_t<Float, State, DerivativeFunction>, Float>
  auto
  operator()(Float time, State const &state, DerivativeFunction derivative, Float time_step) const
      -> derivative_result_t<Float, State, DerivativeFunction>
  {
    using delta_type = derivative_result_t<Float, State, DerivativeFunction>;
    Float constexpr two = static_cast<Float>(2);
    Float constexpr six = static_cast<Float>(6);

    auto const k1 = derivative(time, state);
    auto const k2 = derivative(time + time_step / two, state + k1 * (time_step / two));
    auto const k3 = derivative(time + time_step / two, state + k2 * (time_step / two));
    auto const k4 = derivative(time + time_step, state + k3 * time_step);

    return (k1 + k2 * two + k3 * two + k4) * (time_step / six);
  }
};

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_RK4_HPP
