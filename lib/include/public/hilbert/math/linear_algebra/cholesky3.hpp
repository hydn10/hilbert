#ifndef HILBERT_MATH_LINEAR_ALGEBRA_CHOLESKY3_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_CHOLESKY3_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix3.hpp>
#include <hilbert/math/linear_algebra/vector3.hpp>

#include <cmath>
#include <stdexcept>


namespace hilbert::math
{

template<supported_float Float>
class cholesky_factor3
{
  Float l00_;
  Float l10_;
  Float l11_;
  Float l20_;
  Float l21_;
  Float l22_;

  explicit constexpr cholesky_factor3(Float l00, Float l10, Float l11, Float l20, Float l21, Float l22) noexcept
      : l00_{l00}
      , l10_{l10}
      , l11_{l11}
      , l20_{l20}
      , l21_{l21}
      , l22_{l22}
  {
  }

  template<supported_float OtherFloat>
  friend cholesky_factor3<OtherFloat>
  cholesky_decompose(symmetric_matrix3<OtherFloat> const &matrix);

public:
  [[nodiscard]]
  vector3<Float>
  solve(vector3<Float> const &right_hand_side) const
  {
    auto const y0 = right_hand_side.first() / l00_;
    auto const y1 = (right_hand_side.second() - l10_ * y0) / l11_;
    auto const y2 = (right_hand_side.third() - l20_ * y0 - l21_ * y1) / l22_;

    auto const x2 = y2 / l22_;
    auto const x1 = (y1 - l21_ * x2) / l11_;
    auto const x0 = (y0 - l10_ * x1 - l20_ * x2) / l00_;

    vector3<Float> const result{x0, x1, x2};
    if (!std::isfinite(result.first()) || !std::isfinite(result.second()) || !std::isfinite(result.third()))
    {
      throw std::invalid_argument{"Cholesky solve produced non-finite values"};
    }
    return result;
  }
};


template<supported_float Float>
cholesky_factor3<Float>
cholesky_decompose(symmetric_matrix3<Float> const &matrix)
{
  auto const require_positive_finite = [](Float value)
  {
    if (!std::isfinite(value) || value <= 0)
    {
      throw std::invalid_argument{"matrix is not symmetric positive definite"};
    }
    return value;
  };

  auto const l00 = std::sqrt(require_positive_finite(matrix.m00()));
  auto const l10 = matrix.m10() / l00;
  auto const l20 = matrix.m20() / l00;
  auto const l11_squared = matrix.m11() - l10 * l10;
  auto const l11 = std::sqrt(require_positive_finite(l11_squared));
  auto const l21 = (matrix.m21() - l20 * l10) / l11;
  auto const l22_squared = matrix.m22() - l20 * l20 - l21 * l21;
  auto const l22 = std::sqrt(require_positive_finite(l22_squared));

  if (!std::isfinite(l10) || !std::isfinite(l20) || !std::isfinite(l21) || !std::isfinite(l22))
  {
    throw std::invalid_argument{"Cholesky factor contains non-finite values"};
  }

  return cholesky_factor3<Float>{l00, l10, l11, l20, l21, l22};
}

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_CHOLESKY3_HPP
