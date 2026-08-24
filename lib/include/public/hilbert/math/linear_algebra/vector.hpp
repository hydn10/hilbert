#ifndef HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/signature.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>


namespace hilbert::math
{

template<supported_float Float, coordinate_signature Signature>
class vector
{
  std::array<Float, Signature::size> values_{};

public:
  using signature_type = Signature;
  static constexpr std::size_t size = Signature::size;

  constexpr vector() noexcept = default;

  explicit constexpr vector(std::array<Float, size> values) noexcept
      : values_{values}
  {
  }

  template<typename... Values>
  requires(sizeof...(Values) == size) && (std::same_as<std::remove_cvref_t<Values>, Float> && ...)
  explicit constexpr vector(Values &&...values) noexcept
      : values_{static_cast<Float>(std::forward<Values>(values))...}
  {
  }

  [[nodiscard]]
  constexpr std::array<Float, size> const &
  values() const noexcept
  {
    return values_;
  }

  [[nodiscard]]
  constexpr std::array<Float, size> &
  values() noexcept
  {
    return values_;
  }
};


template<std::size_t Index, supported_float Float, coordinate_signature Signature>
requires(Index < Signature::size)
[[nodiscard]]
constexpr Float const &
get(vector<Float, Signature> const &value) noexcept
{
  return std::get<Index>(value.values());
}


template<std::size_t Index, supported_float Float, coordinate_signature Signature>
requires(Index < Signature::size)
[[nodiscard]]
constexpr Float &
get(vector<Float, Signature> &value) noexcept
{
  return std::get<Index>(value.values());
}


template<typename Tag, supported_float Float, coordinate_signature Signature>
requires(Signature::template contains<Tag>())
[[nodiscard]]
constexpr Float const &
get(vector<Float, Signature> const &value) noexcept
{
  return std::get<Signature::template index<Tag>()>(value.values());
}


template<typename Tag, supported_float Float, coordinate_signature Signature>
requires(Signature::template contains<Tag>())
[[nodiscard]]
constexpr Float &
get(vector<Float, Signature> &value) noexcept
{
  return std::get<Signature::template index<Tag>()>(value.values());
}

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP
