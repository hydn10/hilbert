#ifndef HILBERT_MATH_LINEAR_ALGEBRA_VECTOR3_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_VECTOR3_HPP


#include <hilbert/core/supported_float.hpp>


namespace hilbert::math
{

template<supported_float Float>
class vector3
{
  Float first_{};
  Float second_{};
  Float third_{};

public:
  constexpr vector3() noexcept = default;

  constexpr vector3(Float first, Float second, Float third) noexcept
      : first_{first}
      , second_{second}
      , third_{third}
  {
  }

  [[nodiscard]]
  constexpr Float
  first() const noexcept
  {
    return first_;
  }

  [[nodiscard]]
  constexpr Float
  second() const noexcept
  {
    return second_;
  }

  [[nodiscard]]
  constexpr Float
  third() const noexcept
  {
    return third_;
  }
};

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_VECTOR3_HPP
