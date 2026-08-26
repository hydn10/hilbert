#ifndef HILBERT_PHASE_SCAN_ESTIMATORS_LEAST_SQUARES_ESTIMATE_HPP
#define HILBERT_PHASE_SCAN_ESTIMATORS_LEAST_SQUARES_ESTIMATE_HPP


#include <hilbert/analysis/least_squares/least_squares_products.hpp>
#include <hilbert/analysis/sinusoidal/sinusoidal_fit.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/cholesky_factor3.hpp>

#include <cstddef>
#include <utility>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
class least_squares_estimate
{
  hilbert::analysis::sinusoidal_fit<Float> ground_;
  hilbert::analysis::sinusoidal_fit<Float> tire_force_;
  hilbert::analysis::frequency_response<Float> response_;

public:
  least_squares_estimate(
      hilbert::analysis::sinusoidal_fit<Float> ground, hilbert::analysis::sinusoidal_fit<Float> tire_force);

  [[nodiscard]]
  hilbert::analysis::frequency_response<Float> const &
  response() const noexcept;

  [[nodiscard]]
  hilbert::analysis::sinusoidal_fit<Float> const &
  ground_fit() const noexcept;

  [[nodiscard]]
  hilbert::analysis::sinusoidal_fit<Float> const &
  tire_force_fit() const noexcept;
};


template<hilbert::supported_float Float, hilbert::analysis::sinusoidal_coordinate_signature Signature>
least_squares_estimate<Float>
estimate_phase_scan_by_least_squares(hilbert::analysis::least_squares_products<Float, Signature, 2uz> const &products);


template<hilbert::supported_float Float>
least_squares_estimate<Float>::least_squares_estimate(
    hilbert::analysis::sinusoidal_fit<Float> ground, hilbert::analysis::sinusoidal_fit<Float> tire_force)
    : ground_{std::move(ground)}
    , tire_force_{std::move(tire_force)}
    , response_{hilbert::analysis::make_frequency_response(tire_force_, ground_)}
{
}


template<hilbert::supported_float Float>
hilbert::analysis::frequency_response<Float> const &
least_squares_estimate<Float>::response() const noexcept
{
  return response_;
}


template<hilbert::supported_float Float>
hilbert::analysis::sinusoidal_fit<Float> const &
least_squares_estimate<Float>::ground_fit() const noexcept
{
  return ground_;
}


template<hilbert::supported_float Float>
hilbert::analysis::sinusoidal_fit<Float> const &
least_squares_estimate<Float>::tire_force_fit() const noexcept
{
  return tire_force_;
}


template<hilbert::supported_float Float, hilbert::analysis::sinusoidal_coordinate_signature Signature>
least_squares_estimate<Float>
estimate_phase_scan_by_least_squares(hilbert::analysis::least_squares_products<Float, Signature, 2uz> const &products)
{
  auto const factor = hilbert::math::cholesky_decompose(products.gram());

  auto ground_fit =
      hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(factor.solve(products.template projection<0>()));
  auto tire_force_fit =
      hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(factor.solve(products.template projection<1>()));

  return {std::move(ground_fit), std::move(tire_force_fit)};
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ESTIMATORS_LEAST_SQUARES_ESTIMATE_HPP
