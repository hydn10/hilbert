#ifndef HILBERT_MATH_LINEAR_ALGEBRA_SIGNATURE_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_SIGNATURE_HPP


#include <concepts>
#include <cstddef>
#include <type_traits>


namespace hilbert::math
{

namespace detail
{

template<typename>
inline constexpr bool dependent_false = false;

template<typename Tag>
consteval std::size_t
signature_index() noexcept
{
  static_assert(dependent_false<Tag>, "signature does not contain the requested tag");
  return 0uz;
}

template<typename Tag, typename First, typename... Rest>
consteval std::size_t
signature_index() noexcept
{
  if constexpr (std::same_as<Tag, First>)
  {
    return 0uz;
  }
  else
  {
    return 1uz + signature_index<Tag, Rest...>();
  }
}

template<typename... Tags>
struct are_unique : std::true_type
{
};

template<typename First, typename... Rest>
struct are_unique<First, Rest...>
    : std::bool_constant<((!std::same_as<First, Rest>) && ...) && are_unique<Rest...>::value>
{
};

} // namespace detail


template<typename... Tags>
struct signature
{
  static_assert(detail::are_unique<Tags...>::value, "signature tags must be unique");

  static constexpr std::size_t size = sizeof...(Tags);

  template<typename Tag>
  static consteval bool contains() noexcept
  {
    return (std::same_as<Tag, Tags> || ...);
  }

  template<typename Tag>
  static consteval std::size_t index() noexcept
  {
    static_assert(contains<Tag>(), "signature does not contain the requested tag");
    return detail::signature_index<Tag, Tags...>();
  }
};


template<typename Signature>
struct dual;


template<typename Type>
struct is_signature : std::false_type
{
};

template<typename... Tags>
struct is_signature<signature<Tags...>> : std::true_type
{
};


template<typename Type>
struct is_dual_signature : std::false_type
{
};

template<typename Signature>
struct is_dual_signature<dual<Signature>> : is_signature<Signature>
{
};


template<typename Type>
concept signature_type = is_signature<std::remove_cvref_t<Type>>::value;


template<typename Type>
concept coordinate_signature =
    signature_type<Type> || is_dual_signature<std::remove_cvref_t<Type>>::value;


template<typename Signature>
struct dual
{
  static_assert(coordinate_signature<Signature>, "dual requires a coordinate signature");

  using primal_signature = std::remove_cvref_t<Signature>;

  static constexpr std::size_t size = primal_signature::size;

  template<typename Tag>
  static consteval bool contains() noexcept
  {
    return primal_signature::template contains<Tag>();
  }

  template<typename Tag>
  static consteval std::size_t index() noexcept
  {
    return primal_signature::template index<Tag>();
  }
};


template<typename Signature>
using dual_signature_t = dual<std::remove_cvref_t<Signature>>;


template<typename Signature, typename Tag>
inline constexpr bool contains_tag_v = std::remove_cvref_t<Signature>::template contains<Tag>();

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_SIGNATURE_HPP
