#ifndef HILBERT_SIMULATION_INTEGRATORS_RK4_HPP
#define HILBERT_SIMULATION_INTEGRATORS_RK4_HPP


#include <hilbert/simulation/core/concepts.hpp>

#include <concepts>


namespace hilbert::simulation::integrators
{

template<std::floating_point Float, typename State, dynamics_for<Float, State> Dynamics>
class rk4
{
public:
  using delta_type = dynamics_delta_t<Dynamics, Float, State>;

  delta_type
  operator()(Float time, State const &state, Dynamics const &dynamics, Float time_step) const;
};


template<std::floating_point Float, typename State, dynamics_for<Float, State> Dynamics>
rk4<Float, State, Dynamics>::delta_type
rk4<Float, State, Dynamics>::operator()(Float time, State const &state, Dynamics const &dynamics, Float time_step) const
{
  auto constexpr two = static_cast<Float>(2);
  auto constexpr six = static_cast<Float>(6);

  auto const k1 = dynamics.derivative(time, state);
  auto const k2 = dynamics.derivative(time + time_step / two, state + k1 * (time_step / two));
  auto const k3 = dynamics.derivative(time + time_step / two, state + k2 * (time_step / two));
  auto const k4 = dynamics.derivative(time + time_step, state + k3 * time_step);

  return (k1 + k2 * two + k3 * two + k4) * (time_step / six);
}

} // namespace hilbert::simulation::integrators

#endif // HILBERT_SIMULATION_INTEGRATORS_RK4_HPP
