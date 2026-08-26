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

template<typename Matrix>
struct symmetric_matrix_access;

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
  friend struct detail::symmetric_matrix_access<symmetric_matrix<Float, RowSignature, ColumnSignature>>;

  explicit constexpr symmetric_matrix(std::array<Float, lower_triangle_size> lower_triangle) noexcept;

public:
  static constexpr symmetric_matrix
  from_lower_triangle(std::array<Float, lower_triangle_size> lower_triangle) noexcept;

  template<std::same_as<Float>... Values>
  requires(sizeof...(Values) == ColumnSignature::size * (ColumnSignature::size + 1uz) / 2uz)
  static constexpr symmetric_matrix
  from_lower_triangle(Values... values) noexcept;
};


namespace detail
{

template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
struct symmetric_matrix_access<symmetric_matrix<Float, RowSignature, ColumnSignature>>
{
  template<std::size_t Row, std::size_t Column>
  [[nodiscard]]
  static constexpr Float const &
  get(symmetric_matrix<Float, RowSignature, ColumnSignature> const &matrix) noexcept;

  template<std::size_t Row, std::size_t Column>
  [[nodiscard]]
  static constexpr Float &
  get(symmetric_matrix<Float, RowSignature, ColumnSignature> &matrix) noexcept;
};

} // namespace detail


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


namespace detail
{

template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
template<std::size_t Row, std::size_t Column>
constexpr Float const &
symmetric_matrix_access<symmetric_matrix<Float, RowSignature, ColumnSignature>>::get(
    symmetric_matrix<Float, RowSignature, ColumnSignature> const &matrix) noexcept
{
  return std::get<symmetric_matrix_index<Row, Column>()>(matrix.lower_triangle_);
}


template<
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
template<std::size_t Row, std::size_t Column>
constexpr Float &
symmetric_matrix_access<symmetric_matrix<Float, RowSignature, ColumnSignature>>::get(
    symmetric_matrix<Float, RowSignature, ColumnSignature> &matrix) noexcept
{
  return std::get<symmetric_matrix_index<Row, Column>()>(matrix.lower_triangle_);
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
    std::size_t Row,
    std::size_t Column,
    supported_float Float,
    coordinate_signature RowSignature,
    symmetric_signature_pair<RowSignature> ColumnSignature>
requires(Row < ColumnSignature::size) && (Column < ColumnSignature::size)
constexpr Float const &
get(symmetric_matrix<Float, RowSignature, ColumnSignature> const &matrix) noexcept
{
  return detail::symmetric_matrix_access<
      symmetric_matrix<Float, RowSignature, ColumnSignature>>::template get<Row, Column>(matrix);
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
  return detail::symmetric_matrix_access<
      symmetric_matrix<Float, RowSignature, ColumnSignature>>::template get<Row, Column>(matrix);
}


template<supported_float Float, coordinate_signature_for_size<3uz> Signature>
using symmetric_matrix3 = symmetric_matrix<Float, dual_signature_t<Signature>, Signature>;

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX_HPP
