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

namespace detail
{

template<typename Vector>
struct vector_access;

} // namespace detail


template<typename Value, typename Expected>
concept same_as_unqualified = std::same_as<std::remove_cvref_t<Value>, Expected>;


template<supported_float Float, coordinate_signature Signature>
class vector
{
  std::array<Float, Signature::size> values_{};
  friend struct detail::vector_access<vector<Float, Signature>>;

public:
  using signature_type = Signature;
  static constexpr std::size_t size = Signature::size;

  constexpr vector() noexcept;

  explicit constexpr vector(std::array<Float, size> values) noexcept;

  template<same_as_unqualified<Float>... Values>
  requires(sizeof...(Values) == Signature::size)
  explicit constexpr vector(Values &&...values) noexcept;
};


namespace detail
{

template<supported_float Float, coordinate_signature Signature>
struct vector_access<vector<Float, Signature>>
{
  template<std::size_t Index>
  [[nodiscard]]
  static constexpr Float const &
  get(vector<Float, Signature> const &value) noexcept;

  template<std::size_t Index>
  [[nodiscard]]
  static constexpr Float &
  get(vector<Float, Signature> &value) noexcept;
};

} // namespace detail


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


namespace detail
{

template<supported_float Float, coordinate_signature Signature>
template<std::size_t Index>
constexpr Float const &
vector_access<vector<Float, Signature>>::get(vector<Float, Signature> const &value) noexcept
{
  return std::get<Index>(value.values_);
}


template<supported_float Float, coordinate_signature Signature>
template<std::size_t Index>
constexpr Float &
vector_access<vector<Float, Signature>>::get(vector<Float, Signature> &value) noexcept
{
  return std::get<Index>(value.values_);
}

} // namespace detail


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
  return detail::vector_access<vector<Float, Signature>>::template get<Index>(value);
}


template<std::size_t Index, supported_float Float, coordinate_signature Signature>
requires(Index < Signature::size)
constexpr Float &
get(vector<Float, Signature> &value) noexcept
{
  return detail::vector_access<vector<Float, Signature>>::template get<Index>(value);
}


template<typename Tag, supported_float Float, signature_contains<Tag> Signature>
constexpr Float const &
get(vector<Float, Signature> const &value) noexcept
{
  return detail::vector_access<vector<Float, Signature>>::template get<Signature::template index<Tag>()>(value);
}


template<typename Tag, supported_float Float, signature_contains<Tag> Signature>
constexpr Float &
get(vector<Float, Signature> &value) noexcept
{
  return detail::vector_access<vector<Float, Signature>>::template get<Signature::template index<Tag>()>(value);
}


namespace detail
{

template<supported_float Float, signature_type Signature, std::size_t... Indices>
[[nodiscard]]
constexpr Float
dual_pairing(
    vector<Float, dual_signature_t<Signature>> const &dual,
    vector<Float, Signature> const &primal,
    [[maybe_unused]] std::index_sequence<Indices...> indices) noexcept
{
  return (Float{} + ... + (get<Indices>(dual) * get<Indices>(primal)));
}

} // namespace detail


template<supported_float Float, signature_type Signature>
[[nodiscard]]
constexpr Float
dual_pairing(vector<Float, dual_signature_t<Signature>> const &dual, vector<Float, Signature> const &primal) noexcept
{
  return detail::dual_pairing(dual, primal, std::make_index_sequence<Signature::size>{});
}


template<supported_float Float, coordinate_signature_for_size<3uz> Signature>
using vector3 = vector<Float, Signature>;

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_VECTOR_HPP
