#ifndef HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_EIGENVALUES3_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_EIGENVALUES3_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/signature.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>


namespace hilbert::math
{

template<supported_float Float, signature_for_size<3uz> Signature>
[[nodiscard]]
std::array<Float, 3uz>
symmetric_eigenvalues(symmetric_matrix<Float, dual_signature_t<Signature>, Signature> const &matrix);


template<supported_float Float, signature_for_size<3uz> Signature>
std::array<Float, 3uz>
symmetric_eigenvalues(symmetric_matrix<Float, dual_signature_t<Signature>, Signature> const &matrix)
{
  auto const a00 = get<0, 0>(matrix);
  auto const a10 = get<1, 0>(matrix);
  auto const a11 = get<1, 1>(matrix);
  auto const a20 = get<2, 0>(matrix);
  auto const a21 = get<2, 1>(matrix);
  auto const a22 = get<2, 2>(matrix);

  auto const mean_eigenvalue = (a00 + a11 + a22) / static_cast<Float>(3);
  auto const centered00 = a00 - mean_eigenvalue;
  auto const centered11 = a11 - mean_eigenvalue;
  auto const centered22 = a22 - mean_eigenvalue;
  auto const centered_squared_norm = centered00 * centered00 + centered11 * centered11 + centered22 * centered22 +
                                     static_cast<Float>(2) * (a10 * a10 + a20 * a20 + a21 * a21);

  if (centered_squared_norm == 0)
  {
    return {mean_eigenvalue, mean_eigenvalue, mean_eigenvalue};
  }

  auto const scale = std::sqrt(centered_squared_norm / static_cast<Float>(6));
  auto const b00 = centered00 / scale;
  auto const b10 = a10 / scale;
  auto const b11 = centered11 / scale;
  auto const b20 = a20 / scale;
  auto const b21 = a21 / scale;
  auto const b22 = centered22 / scale;
  auto const determinant =
      b00 * (b11 * b22 - b21 * b21) - b10 * (b10 * b22 - b21 * b20) + b20 * (b10 * b21 - b11 * b20);
  auto const half_determinant =
      std::clamp(determinant / static_cast<Float>(2), static_cast<Float>(-1), static_cast<Float>(1));
  auto const angle = std::acos(half_determinant) / static_cast<Float>(3);
  constexpr auto two_pi_over_three = static_cast<Float>(2) * std::numbers::pi_v<Float> / static_cast<Float>(3);

  std::array eigenvalues{
      mean_eigenvalue + static_cast<Float>(2) * scale * std::cos(angle),
      mean_eigenvalue + static_cast<Float>(2) * scale * std::cos(angle + two_pi_over_three),
      mean_eigenvalue + static_cast<Float>(2) * scale * std::cos(angle - two_pi_over_three),
  };
  std::ranges::sort(eigenvalues);
  return eigenvalues;
}

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_EIGENVALUES3_HPP
