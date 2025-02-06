#ifndef HILBERTCLI_RK4_HPP
#define HILBERTCLI_RK4_HPP


#include <concepts>


namespace hilbertcli
{

// clang-format off
template<typename Float, typename Z, typename D>
concept state_space = std::floating_point<Float> && requires(Z z, D d, Float f) {
  { z + d } -> std::same_as<Z>;

  { d * f } -> std::same_as<D>;
  { f * d } -> std::same_as<D>;

  { d + d } -> std::same_as<D>;
};


template<typename Float, typename F, typename Z>
concept state_derivative_func = requires(F f, Float t, Z const &z) {
  typename std::invoke_result_t<F, Float, Z>;
} && state_space<Float, Z, std::invoke_result_t<F, Float, Z>>;
// clang-format on


template<std::floating_point Float, typename Z, typename Derivate>
requires state_derivative_func<Float, Derivate, Z>
auto
rk4_delta(Float t, Z const &z, Derivate derivate, Float time_step);


template<std::floating_point Float, typename Z, typename Derivate>
requires state_derivative_func<Float, Derivate, Z>
auto
rk4_delta(Float t, Z const &z, Derivate derivate, Float time_step)
{
  auto const h = time_step;

  auto k1 = derivate(t, z);
  auto k2 = derivate(t + h / 2, z + k1 * (h / 2));
  auto k3 = derivate(t + h / 2, z + k2 * (h / 2));
  auto k4 = derivate(t + h, z + k3 * h);

  return (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
}

} // namespace hilbertcli

#endif
