#ifndef HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/signature.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>


namespace hilbert::math
{

namespace detail
{

template<std::size_t Row, std::size_t Column>
consteval std::size_t
symmetric_matrix_index() noexcept;

} // namespace detail

template<typename ColumnSignature, typename RowSignature>
concept symmetric_signature_pair = coordinate_signature<ColumnSignature> && coordinate_signature<RowSignature> &&
                                   std::same_as<RowSignature, dual_signature_t<ColumnSignature>>;


template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
class symmetric_matrix
{
public:
  using row_signature = RowSignature;
  using column_signature = ColumnSignature;
  static constexpr std::size_t size = ColumnSignature::size;
  static constexpr std::size_t lower_triangle_size = size * (size + 1uz) / 2uz;

private:
  std::array<Float, lower_triangle_size> lower_triangle_{};

  explicit constexpr symmetric_matrix(std::array<Float, lower_triangle_size> lower_triangle) noexcept;

public:
  static constexpr symmetric_matrix
  from_lower_triangle(std::array<Float, lower_triangle_size> lower_triangle) noexcept;

  template<std::same_as<Float>... Values>
  requires(sizeof...(Values) == ColumnSignature::size * (ColumnSignature::size + 1uz) / 2uz)
  static constexpr symmetric_matrix
  from_lower_triangle(Values... values) noexcept;

  [[nodiscard]]
  constexpr std::array<Float, lower_triangle_size> const &
  values() const noexcept;

  [[nodiscard]]
  constexpr std::array<Float, lower_triangle_size> &
  values() noexcept;
};


template<
    std::size_t Row,
    std::size_t Column,
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
requires(Row < ColumnSignature::size) && (Column < ColumnSignature::size)
[[nodiscard]]
constexpr Float const &
get(symmetric_matrix<Float, RowSignature, ColumnSignature> const &matrix) noexcept;


template<
    std::size_t Row,
    std::size_t Column,
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
requires(Row < ColumnSignature::size) && (Column < ColumnSignature::size)
[[nodiscard]]
constexpr Float &
get(symmetric_matrix<Float, RowSignature, ColumnSignature> &matrix) noexcept;


namespace detail
{

template<std::size_t Row, std::size_t Column>
consteval std::size_t
symmetric_matrix_index() noexcept
{
  if constexpr (Row >= Column)
  {
    return Row * (Row + 1uz) / 2uz + Column;
  }
  else
  {
    return Column * (Column + 1uz) / 2uz + Row;
  }
}

} // namespace detail


template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
constexpr symmetric_matrix<Float, RowSignature, ColumnSignature>::symmetric_matrix(
    std::array<Float, lower_triangle_size> lower_triangle) noexcept
    : lower_triangle_{lower_triangle}
{
}


template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
constexpr symmetric_matrix<Float, RowSignature, ColumnSignature>
symmetric_matrix<Float, RowSignature, ColumnSignature>::from_lower_triangle(
    std::array<Float, lower_triangle_size> lower_triangle) noexcept
{
  return symmetric_matrix{lower_triangle};
}


template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
template<std::same_as<Float>... Values>
requires(sizeof...(Values) == ColumnSignature::size * (ColumnSignature::size + 1uz) / 2uz)
constexpr symmetric_matrix<Float, RowSignature, ColumnSignature>
symmetric_matrix<Float, RowSignature, ColumnSignature>::from_lower_triangle(Values... values) noexcept
{
  return symmetric_matrix{std::array<Float, lower_triangle_size>{static_cast<Float>(values)...}};
}


template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
constexpr std::array<Float, symmetric_matrix<Float, RowSignature, ColumnSignature>::lower_triangle_size> const &
symmetric_matrix<Float, RowSignature, ColumnSignature>::values() const noexcept
{
  return lower_triangle_;
}


template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
constexpr std::array<Float, symmetric_matrix<Float, RowSignature, ColumnSignature>::lower_triangle_size> &
symmetric_matrix<Float, RowSignature, ColumnSignature>::values() noexcept
{
  return lower_triangle_;
}


template<
    std::size_t Row,
    std::size_t Column,
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
requires(Row < ColumnSignature::size) && (Column < ColumnSignature::size)
constexpr Float const &
get(symmetric_matrix<Float, RowSignature, ColumnSignature> const &matrix) noexcept
{
  return std::get<detail::symmetric_matrix_index<Row, Column>()>(matrix.values());
}


template<
    std::size_t Row,
    std::size_t Column,
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
requires(Row < ColumnSignature::size) && (Column < ColumnSignature::size)
constexpr Float &
get(symmetric_matrix<Float, RowSignature, ColumnSignature> &matrix) noexcept
{
  return std::get<detail::symmetric_matrix_index<Row, Column>()>(matrix.values());
}

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX_HPP
