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

template<typename Value, typename Expected>
concept same_as_unqualified = std::same_as<std::remove_cvref_t<Value>, Expected>;


template<supported_float Float, coordinate_signature Signature>
class vector
{
  std::array<Float, Signature::size> values_{};
  using value_array = std::array<Float, Signature::size>;

public:
  using signature_type = Signature;
  static constexpr std::size_t size = Signature::size;

  constexpr vector() noexcept;

  explicit constexpr vector(std::array<Float, size> values) noexcept;

  template<same_as_unqualified<Float>... Values>
  requires(sizeof...(Values) == Signature::size)
  explicit constexpr vector(Values &&...values) noexcept;

  [[nodiscard]]
  constexpr value_array const &
  values() const noexcept;

  [[nodiscard]]
  constexpr value_array &
  values() noexcept;
};


template<supported_float Float, coordinate_signature Signature>
constexpr vector<Float, Signature>::value_array const &
vector<Float, Signature>::values() const noexcept
{
  return values_;
}


template<supported_float Float, coordinate_signature Signature>
constexpr vector<Float, Signature>::value_array &
vector<Float, Signature>::values() noexcept
{
  return values_;
}

template<std::size_t Index, supported_float Float, coordinate_signature Signature>
requires(Index < Signature::size)
[[nodiscard]]
constexpr Float const &
get(vector<Float, Signature> const &value) noexcept;


template<std::size_t Index, supported_float Float, coordinate_signature Signature>
requires(Index < Signature::size)
[[nodiscard]]
constexpr Float &
get(vector<Float, Signature> &value) noexcept;


template<typename Tag, supported_float Float, signature_contains<Tag> Signature>
[[nodiscard]]
constexpr Float const &
get(vector<Float, Signature> const &value) noexcept;


template<typename Tag, supported_float Float, signature_contains<Tag> Signature>
[[nodiscard]]
constexpr Float &
get(vector<Float, Signature> &value) noexcept;


template<supported_float Float, coordinate_signature Signature>
constexpr vector<Float, Signature>::vector() noexcept = default;


template<supported_float Float, coordinate_signature Signature>
constexpr vector<Float, Signature>::vector(std::array<Float, size> values) noexcept
    : values_{values}
{
}


template<supported_float Float, coordinate_signature Signature>
template<same_as_unqualified<Float>... Values>
requires(sizeof...(Values) == Signature::size)
constexpr vector<Float, Signature>::vector(Values &&...values) noexcept
    : values_{static_cast<Float>(std::forward<Values>(values))...}
{
}


template<std::size_t Index, supported_float Float, coordinate_signature Signature>
requires(Index < Signature::size)
constexpr Float const &
get(vector<Float, Signature> const &value) noexcept
{
  return std::get<Index>(value.values());
}


template<std::size_t Index, supported_float Float, coordinate_signature Signature>
requires(Index < Signature::size)
constexpr Float &
get(vector<Float, Signature> &value) noexcept
{
  return std::get<Index>(value.values());
}


template<typename Tag, supported_float Float, signature_contains<Tag> Signature>
constexpr Float const &
get(vector<Float, Signature> const &value) noexcept
{
  return std::get<Signature::template index<Tag>()>(value.values());
}


template<typename Tag, supported_float Float, signature_contains<Tag> Signature>
constexpr Float &
get(vector<Float, Signature> &value) noexcept
{
  return std::get<Signature::template index<Tag>()>(value.values());
}

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP
