#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_VALUE_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_VALUE_HPP


#include <hilbert/core/supported_float.hpp>

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>


namespace hilbert::analysis
{

struct one_t
{
};


inline constexpr one_t constant_one;


template<supported_float Float>
constexpr one_t
evaluate_basis_element([[maybe_unused]] one_t element, [[maybe_unused]] Float argument) noexcept;


template<supported_float Float, typename BasisElement>
requires std::invocable<BasisElement const &, Float>
std::remove_cvref_t<std::invoke_result_t<BasisElement const &, Float>>
evaluate_basis_element(BasisElement const &element, Float argument);


template<supported_float Float, typename BasisElement>
using basis_element_value_t =
    std::remove_cvref_t<decltype(evaluate_basis_element(std::declval<BasisElement const &>(), std::declval<Float>()))>;


template<supported_float Float>
constexpr Float
pointwise_multiply(Float left, Float right) noexcept;


template<supported_float Float>
constexpr Float
pointwise_multiply([[maybe_unused]] one_t left, Float value) noexcept;


template<supported_float Float>
constexpr Float
pointwise_multiply(Float value, [[maybe_unused]] one_t right) noexcept;


constexpr one_t
pointwise_multiply([[maybe_unused]] one_t left, [[maybe_unused]] one_t right) noexcept;


template<supported_float Float>
constexpr one_t
evaluate_basis_element([[maybe_unused]] one_t element, [[maybe_unused]] Float argument) noexcept
{
  return {};
}


template<supported_float Float, typename BasisElement>
requires std::invocable<BasisElement const &, Float>
std::remove_cvref_t<std::invoke_result_t<BasisElement const &, Float>>
evaluate_basis_element(BasisElement const &element, Float argument)
{
  return std::invoke(element, argument);
}


template<supported_float Float>
constexpr Float
pointwise_multiply(Float left, Float right) noexcept
{
  return left * right;
}


template<supported_float Float>
constexpr Float
pointwise_multiply([[maybe_unused]] one_t left, Float value) noexcept
{
  return value;
}


template<supported_float Float>
constexpr Float
pointwise_multiply(Float value, [[maybe_unused]] one_t right) noexcept
{
  return value;
}


constexpr one_t
pointwise_multiply([[maybe_unused]] one_t left, [[maybe_unused]] one_t right) noexcept
{
  return {};
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_VALUE_HPP
