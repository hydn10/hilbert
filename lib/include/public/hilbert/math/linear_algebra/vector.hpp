#ifndef HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP


#include <hilbert/core/supported_float.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>


namespace hilbert::math
{

template<supported_float Float, std::size_t Size>
class vector;


template<std::size_t Index, supported_float Float, std::size_t Size>
requires(Index < Size)
constexpr Float const &
get(vector<Float, Size> const &value) noexcept;


template<std::size_t Index, supported_float Float, std::size_t Size>
requires(Index < Size)
constexpr Float &
get(vector<Float, Size> &value) noexcept;


template<supported_float Float, std::size_t Size>
class vector
{
  std::array<Float, Size> values_{};

  template<std::size_t Index, supported_float OtherFloat, std::size_t OtherSize>
  requires(Index < OtherSize)
  friend constexpr OtherFloat const &
  get(vector<OtherFloat, OtherSize> const &value) noexcept;

  template<std::size_t Index, supported_float OtherFloat, std::size_t OtherSize>
  requires(Index < OtherSize)
  friend constexpr OtherFloat &
  get(vector<OtherFloat, OtherSize> &value) noexcept;

public:
  static constexpr std::size_t size = Size;

  constexpr vector() noexcept = default;

  explicit constexpr vector(std::array<Float, Size> values) noexcept
      : values_{values}
  {
  }

  template<typename... Values>
  requires(sizeof...(Values) == Size) && (std::same_as<std::remove_cvref_t<Values>, Float> && ...)
  explicit constexpr vector(Values &&...values) noexcept
      : values_{static_cast<Float>(std::forward<Values>(values))...}
  {
  }

  [[nodiscard]]
  constexpr std::array<Float, Size> const &
  values() const noexcept
  {
    return values_;
  }

  [[nodiscard]]
  constexpr std::array<Float, Size> &
  values() noexcept
  {
    return values_;
  }
};


template<std::size_t Index, supported_float Float, std::size_t Size>
requires(Index < Size)
[[nodiscard]]
constexpr Float const &
get(vector<Float, Size> const &value) noexcept
{
  return value.values_.at(Index);
}


template<std::size_t Index, supported_float Float, std::size_t Size>
requires(Index < Size)
[[nodiscard]]
constexpr Float &
get(vector<Float, Size> &value) noexcept
{
  return value.values_.at(Index);
}

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP
