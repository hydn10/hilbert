#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_HPP


#include <hilbert/analysis/least_squares/basis_value.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/signature.hpp>

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>


namespace hilbert::analysis
{

template<typename Value, typename Float>
concept basis_value = supported_float<Float> && (std::same_as<std::remove_cvref_t<Value>, Float> ||
                                                 std::same_as<std::remove_cvref_t<Value>, one_t>);


template<typename... Values>
class basis_row;


template<std::size_t Index, typename... Values>
requires(Index < sizeof...(Values))
constexpr std::tuple_element_t<Index, std::tuple<Values...>> const &
get(basis_row<Values...> const &row) noexcept;


template<std::size_t Index, typename... Values>
requires(Index < sizeof...(Values))
constexpr std::tuple_element_t<Index, std::tuple<Values...>> &
get(basis_row<Values...> &row) noexcept;


template<typename... Values>
class basis_row
{
  std::tuple<Values...> values_;

  template<std::size_t Index, typename... OtherValues>
  requires(Index < sizeof...(OtherValues))
  friend constexpr std::tuple_element_t<Index, std::tuple<OtherValues...>> const &
  get(basis_row<OtherValues...> const &row) noexcept;

  template<std::size_t Index, typename... OtherValues>
  requires(Index < sizeof...(OtherValues))
  friend constexpr std::tuple_element_t<Index, std::tuple<OtherValues...>> &
  get(basis_row<OtherValues...> &row) noexcept;

public:
  static constexpr std::size_t size = sizeof...(Values);

  explicit constexpr basis_row(Values... values);

  template<std::size_t Index>
  requires(Index < size)
  using value_type = std::tuple_element_t<Index, std::tuple<Values...>>;
};


template<typename... Values>
constexpr basis_row<Values...>::basis_row(Values... values)
    : values_{std::move(values)...}
{
}


template<std::size_t Index, typename... Values>
requires(Index < sizeof...(Values))
[[nodiscard]]
constexpr std::tuple_element_t<Index, std::tuple<Values...>> const &
get(basis_row<Values...> const &row) noexcept
{
  return std::get<Index>(row.values_);
}


template<std::size_t Index, typename... Values>
requires(Index < sizeof...(Values))
[[nodiscard]]
constexpr std::tuple_element_t<Index, std::tuple<Values...>> &
get(basis_row<Values...> &row) noexcept
{
  return std::get<Index>(row.values_);
}


template<typename Function>
concept basis_function = requires { typename std::remove_cvref_t<Function>::tag_type; };


template<typename Function, typename Float>
concept basis_function_for =
    basis_function<Function> && supported_float<Float> && requires(Function const &function, Float argument) {
      { evaluate_basis_element(function, argument) } -> basis_value<Float>;
    };


template<typename Float, typename... Functions>
concept basis_functions_for = supported_float<Float> && (basis_function_for<Functions, Float> && ...);


namespace detail
{

template<typename Row, typename Float, std::size_t... Indices>
consteval bool
basis_row_elements_for([[maybe_unused]] std::index_sequence<Indices...> indices)
{
  return (basis_value<typename Row::template value_type<Indices>, Float> && ...) &&
  requires(Row const &row)
  {
    (get<Indices>(row), ...);
  };
}

} // namespace detail


template<std::size_t Size, typename Row, typename Float>
concept basis_row_for_size = supported_float<Float> && requires { requires Row::size == Size; } &&
                             detail::basis_row_elements_for<Row, Float>(std::make_index_sequence<Size>{});


template<typename Basis, typename Float>
concept basis_for = supported_float<Float> && requires(Basis const &basis, Float argument) {
  typename Basis::signature_type;
  requires math::signature_type<typename Basis::signature_type>;
  basis(argument);
};


template<typename Basis, std::size_t Size, typename Float>
concept basis_for_size = basis_for<Basis, Float> && requires {
  requires basis_row_for_size<
      Size,
      std::remove_cvref_t<decltype(std::declval<Basis const &>()(std::declval<Float>()))>,
      Float>;
};


template<basis_function... Functions>
class basis
{
  std::tuple<Functions...> functions_;

  template<basis_functions_for<Functions...> Float, std::size_t... Indices>
  [[nodiscard]]
  auto
  make_row(Float argument, [[maybe_unused]] std::index_sequence<Indices...> indices) const;

public:
  static constexpr std::size_t size = sizeof...(Functions);
  using signature_type = math::signature<typename std::remove_cvref_t<Functions>::tag_type...>;

  explicit constexpr basis(Functions... functions);

  template<basis_functions_for<Functions...> Float>
  using row_type = basis_row<basis_element_value_t<Float, Functions>...>;

  template<basis_functions_for<Functions...> Float>
  [[nodiscard]]
  row_type<Float>
  operator()(Float argument) const;
};


template<basis_function... Functions>
template<basis_functions_for<Functions...> Float, std::size_t... Indices>
auto
basis<Functions...>::make_row(Float argument, [[maybe_unused]] std::index_sequence<Indices...> indices) const
{
  return basis_row<basis_element_value_t<Float, Functions>...>{
      evaluate_basis_element(std::get<Indices>(functions_), argument)...};
}


template<basis_function... Functions>
constexpr basis<Functions...>::basis(Functions... functions)
    : functions_{std::move(functions)...}
{
}


template<basis_function... Functions>
template<basis_functions_for<Functions...> Float>
basis<Functions...>::template row_type<Float>
basis<Functions...>::operator()(Float argument) const
{
  return make_row<Float>(argument, std::index_sequence_for<Functions...>{});
}


template<class... Functions>
basis(Functions...) -> basis<Functions...>;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_HPP
