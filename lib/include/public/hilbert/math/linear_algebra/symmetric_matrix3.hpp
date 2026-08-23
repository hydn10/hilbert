#ifndef HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX3_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX3_HPP


#include <hilbert/core/supported_float.hpp>


namespace hilbert::math
{

template<supported_float Float>
class symmetric_matrix3
{
  Float m00_;
  Float m10_;
  Float m11_;
  Float m20_;
  Float m21_;
  Float m22_;

  constexpr symmetric_matrix3(Float m00, Float m10, Float m11, Float m20, Float m21, Float m22) noexcept
      : m00_{m00}
      , m10_{m10}
      , m11_{m11}
      , m20_{m20}
      , m21_{m21}
      , m22_{m22}
  {
  }

public:
  static constexpr symmetric_matrix3
  from_lower_triangle(Float m00, Float m10, Float m11, Float m20, Float m21, Float m22) noexcept
  {
    return symmetric_matrix3{m00, m10, m11, m20, m21, m22};
  }

  [[nodiscard]]
  constexpr Float
  m00() const noexcept
  {
    return m00_;
  }

  [[nodiscard]]
  constexpr Float
  m10() const noexcept
  {
    return m10_;
  }

  [[nodiscard]]
  constexpr Float
  m11() const noexcept
  {
    return m11_;
  }

  [[nodiscard]]
  constexpr Float
  m20() const noexcept
  {
    return m20_;
  }

  [[nodiscard]]
  constexpr Float
  m21() const noexcept
  {
    return m21_;
  }

  [[nodiscard]]
  constexpr Float
  m22() const noexcept
  {
    return m22_;
  }
};

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX3_HPP
