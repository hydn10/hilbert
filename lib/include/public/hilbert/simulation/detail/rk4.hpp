#ifndef HILBERT_SIMULATION_DETAIL_RK4_HPP
#define HILBERT_SIMULATION_DETAIL_RK4_HPP


#include <concepts>


namespace hilbert::simulation::detail
{

template<std::floating_point Float, typename State, typename DerivativeFunction>
auto
rk4_delta(Float time, State const &state, DerivativeFunction derivative, Float time_step)
{
  auto const k1 = derivative(time, state);
  auto const k2 = derivative(time + time_step / 2, state + k1 * (time_step / 2));
  auto const k3 = derivative(time + time_step / 2, state + k2 * (time_step / 2));
  auto const k4 = derivative(time + time_step, state + k3 * time_step);

  return (k1 + k2 * 2 + k3 * 2 + k4) * (time_step / 6);
}

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_RK4_HPP
