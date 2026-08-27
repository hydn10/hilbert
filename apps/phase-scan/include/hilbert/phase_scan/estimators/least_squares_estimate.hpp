#ifndef HILBERT_PHASE_SCAN_ESTIMATORS_LEAST_SQUARES_ESTIMATE_HPP
#define HILBERT_PHASE_SCAN_ESTIMATORS_LEAST_SQUARES_ESTIMATE_HPP


#include <hilbert/analysis/least_squares/least_squares_diagnostics.hpp>
#include <hilbert/analysis/least_squares/least_squares_products.hpp>
#include <hilbert/analysis/least_squares/least_squares_residual_statistics.hpp>
#include <hilbert/analysis/sinusoidal/basis.hpp>
#include <hilbert/analysis/sinusoidal/sinusoidal_fit.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/detail/attributes.hpp>
#include <hilbert/math/linear_algebra/cholesky_factor3.hpp>
#include <hilbert/math/linear_algebra/vector.hpp>

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
struct least_squares_diagnostics
{
  Float basis_condition_number;
  Float ground_normalized_residual;
  Float tire_force_normalized_residual;
};


namespace detail
{

template<
    std::size_t Index,
    hilbert::supported_float Float,
    hilbert::math::signature_type Signature,
    typename Statistics>
requires hilbert::analysis::least_squares_residual_statistics_for<Statistics, Float, 2uz>
[[nodiscard]]
Float
normalized_residual(
    hilbert::analysis::least_squares_products<Float, Signature, 2uz, Statistics> const &products,
    hilbert::math::vector<Float, Signature> const &coefficients);


template<hilbert::supported_float Float, hilbert::math::signature_type Signature, typename Statistics>
requires hilbert::analysis::least_squares_residual_statistics_for<Statistics, Float, 2uz>
[[nodiscard]]
least_squares_diagnostics<Float>
make_least_squares_diagnostics(
    hilbert::analysis::least_squares_products<Float, Signature, 2uz, Statistics> const &products,
    hilbert::math::vector<Float, Signature> const &ground_coefficients,
    hilbert::math::vector<Float, Signature> const &tire_force_coefficients);

} // namespace detail


template<hilbert::supported_float Float, typename Diagnostics = void>
class least_squares_estimate
{
  using diagnostics_storage = std::conditional_t<std::is_void_v<Diagnostics>, std::monostate, Diagnostics>;

  hilbert::analysis::sinusoidal_fit<Float> ground_;
  hilbert::analysis::sinusoidal_fit<Float> tire_force_;
  hilbert::analysis::frequency_response<Float> response_;
  HILBERT_NO_UNIQUE_ADDRESS
  diagnostics_storage diagnostics_;

public:
  least_squares_estimate(
      hilbert::analysis::sinusoidal_fit<Float> ground,
      hilbert::analysis::sinusoidal_fit<Float> tire_force,
      diagnostics_storage diagnostics = {});

  [[nodiscard]]
  hilbert::analysis::frequency_response<Float> const &
  response() const noexcept;

  [[nodiscard]]
  hilbert::analysis::sinusoidal_fit<Float> const &
  ground_fit() const noexcept;

  [[nodiscard]]
  hilbert::analysis::sinusoidal_fit<Float> const &
  tire_force_fit() const noexcept;

  [[nodiscard]]
  auto const &
  diagnostics() const noexcept
  requires(!std::is_void_v<Diagnostics>);
};


template<
    hilbert::supported_float Float,
    hilbert::analysis::sinusoidal_coordinate_signature Signature,
    typename Statistics>
[[nodiscard]]
auto
estimate_phase_scan_by_least_squares(
    hilbert::analysis::least_squares_products<Float, Signature, 2uz, Statistics> const &products);


template<
    std::size_t Index,
    hilbert::supported_float Float,
    hilbert::math::signature_type Signature,
    typename Statistics>
requires hilbert::analysis::least_squares_residual_statistics_for<Statistics, Float, 2uz>
Float
detail::normalized_residual(
    hilbert::analysis::least_squares_products<Float, Signature, 2uz, Statistics> const &products,
    hilbert::math::vector<Float, Signature> const &coefficients)
{
  auto const &projection = products.template projection<Index>();
  auto const &statistics = products.statistics();
  return hilbert::analysis::normalized_least_squares_residual(
      products.gram(),
      projection,
      coefficients,
      std::get<Index>(statistics.response_squared_norms),
      std::get<Index>(statistics.response_centered_squared_norms));
}


template<hilbert::supported_float Float, hilbert::math::signature_type Signature, typename Statistics>
requires hilbert::analysis::least_squares_residual_statistics_for<Statistics, Float, 2uz>
least_squares_diagnostics<Float>
detail::make_least_squares_diagnostics(
    hilbert::analysis::least_squares_products<Float, Signature, 2uz, Statistics> const &products,
    hilbert::math::vector<Float, Signature> const &ground_coefficients,
    hilbert::math::vector<Float, Signature> const &tire_force_coefficients)
{
  return {
      .basis_condition_number = hilbert::analysis::column_normalized_basis_condition_number(products.gram()),
      .ground_normalized_residual = detail::normalized_residual<0>(products, ground_coefficients),
      .tire_force_normalized_residual = detail::normalized_residual<1>(products, tire_force_coefficients),
  };
}


template<hilbert::supported_float Float, typename Diagnostics>
least_squares_estimate<Float, Diagnostics>::least_squares_estimate(
    hilbert::analysis::sinusoidal_fit<Float> ground,
    hilbert::analysis::sinusoidal_fit<Float> tire_force,
    diagnostics_storage diagnostics)
    : ground_{std::move(ground)}
    , tire_force_{std::move(tire_force)}
    , response_{hilbert::analysis::make_frequency_response(tire_force_, ground_)}
    , diagnostics_{std::move(diagnostics)}
{
}


template<hilbert::supported_float Float, typename Diagnostics>
hilbert::analysis::frequency_response<Float> const &
least_squares_estimate<Float, Diagnostics>::response() const noexcept
{
  return response_;
}


template<hilbert::supported_float Float, typename Diagnostics>
hilbert::analysis::sinusoidal_fit<Float> const &
least_squares_estimate<Float, Diagnostics>::ground_fit() const noexcept
{
  return ground_;
}


template<hilbert::supported_float Float, typename Diagnostics>
hilbert::analysis::sinusoidal_fit<Float> const &
least_squares_estimate<Float, Diagnostics>::tire_force_fit() const noexcept
{
  return tire_force_;
}


template<hilbert::supported_float Float, typename Diagnostics>
auto const &
least_squares_estimate<Float, Diagnostics>::diagnostics() const noexcept
requires(!std::is_void_v<Diagnostics>)
{
  return diagnostics_;
}


template<
    hilbert::supported_float Float,
    hilbert::analysis::sinusoidal_coordinate_signature Signature,
    typename Statistics>
auto
estimate_phase_scan_by_least_squares(
    hilbert::analysis::least_squares_products<Float, Signature, 2uz, Statistics> const &products)
{
  auto const factor = hilbert::math::cholesky_decompose(products.gram());
  auto const ground_coefficients = factor.solve(products.template projection<0>());
  auto const tire_force_coefficients = factor.solve(products.template projection<1>());
  auto ground_fit = hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(ground_coefficients);
  auto tire_force_fit = hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(tire_force_coefficients);

  if constexpr (hilbert::analysis::least_squares_residual_statistics_for<Statistics, Float, 2uz>)
  {
    auto diagnostics = detail::make_least_squares_diagnostics(products, ground_coefficients, tire_force_coefficients);
    return least_squares_estimate<Float, least_squares_diagnostics<Float>>{
        std::move(ground_fit), std::move(tire_force_fit), std::move(diagnostics)};
  }
  else
  {
    return least_squares_estimate<Float>{std::move(ground_fit), std::move(tire_force_fit)};
  }
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ESTIMATORS_LEAST_SQUARES_ESTIMATE_HPP
