#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_HPP


#include <hilbert/analysis/least_squares/basis_value.hpp>
#include <hilbert/core/supported_float.hpp>

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

  explicit constexpr basis_row(Values... values)
      : values_{std::move(values)...}
  {
  }

  template<std::size_t Index>
  requires(Index < size)
  using value_type = std::tuple_element_t<Index, std::tuple<Values...>>;
};


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


template<class... Functions>
class basis
{
  std::tuple<Functions...> functions_;

  template<supported_float Float, std::size_t... Indices>
  [[nodiscard]]
  auto
  row_at(Float argument, [[maybe_unused]] std::index_sequence<Indices...> indices) const
  {
    return basis_row<basis_element_value_t<Float, Functions>...>{
        evaluate_basis_element(std::get<Indices>(functions_), argument)...};
  }

public:
  static constexpr std::size_t size = sizeof...(Functions);

  explicit constexpr basis(Functions... functions)
      : functions_{std::move(functions)...}
  {
  }

  template<supported_float Float>
  using row_type = basis_row<basis_element_value_t<Float, Functions>...>;

  template<supported_float Float>
  [[nodiscard]]
  row_type<Float>
  row_at(Float argument) const
  {
    return row_at<Float>(argument, std::index_sequence_for<Functions...>{});
  }
};


template<class... Functions>
basis(Functions...) -> basis<Functions...>;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_BASIS_HPP
