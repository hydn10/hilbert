#ifndef HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX_HPP


#include <hilbert/core/supported_float.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>


namespace hilbert::math
{

template<supported_float Float, std::size_t Size>
class symmetric_matrix;


template<std::size_t Row, std::size_t Column, supported_float Float, std::size_t Size>
requires(Row < Size) && (Column < Size)
constexpr Float const &
get(symmetric_matrix<Float, Size> const &matrix) noexcept;


template<std::size_t Row, std::size_t Column, supported_float Float, std::size_t Size>
requires(Row < Size) && (Column < Size)
constexpr Float &
get(symmetric_matrix<Float, Size> &matrix) noexcept;


template<supported_float Float, std::size_t Size>
class symmetric_matrix
{
public:
  static constexpr std::size_t size = Size;
  static constexpr std::size_t lower_triangle_size = Size * (Size + 1uz) / 2uz;

private:
  std::array<Float, lower_triangle_size> lower_triangle_{};

  template<std::size_t Row, std::size_t Column, supported_float OtherFloat, std::size_t OtherSize>
  requires(Row < OtherSize) && (Column < OtherSize)
  friend constexpr OtherFloat const &
  get(symmetric_matrix<OtherFloat, OtherSize> const &matrix) noexcept;

  template<std::size_t Row, std::size_t Column, supported_float OtherFloat, std::size_t OtherSize>
  requires(Row < OtherSize) && (Column < OtherSize)
  friend constexpr OtherFloat &
  get(symmetric_matrix<OtherFloat, OtherSize> &matrix) noexcept;

  template<std::size_t Row, std::size_t Column>
  static constexpr std::size_t
  lower_triangle_index() noexcept
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

  explicit constexpr symmetric_matrix(std::array<Float, lower_triangle_size> lower_triangle) noexcept
      : lower_triangle_{lower_triangle}
  {
  }

public:
  static constexpr symmetric_matrix
  from_lower_triangle(std::array<Float, lower_triangle_size> lower_triangle) noexcept
  {
    return symmetric_matrix{lower_triangle};
  }

  template<typename... Values>
  requires(sizeof...(Values) == lower_triangle_size) && (std::same_as<std::remove_cvref_t<Values>, Float> && ...)
  static constexpr symmetric_matrix
  from_lower_triangle(Values... values) noexcept
  {
    return symmetric_matrix{std::array<Float, lower_triangle_size>{static_cast<Float>(values)...}};
  }
};


template<std::size_t Row, std::size_t Column, supported_float Float, std::size_t Size>
requires(Row < Size) && (Column < Size)
[[nodiscard]]
constexpr Float const &
get(symmetric_matrix<Float, Size> const &matrix) noexcept
{
  return matrix.lower_triangle_.at(matrix.template lower_triangle_index<Row, Column>());
}


template<std::size_t Row, std::size_t Column, supported_float Float, std::size_t Size>
requires(Row < Size) && (Column < Size)
[[nodiscard]]
constexpr Float &
get(symmetric_matrix<Float, Size> &matrix) noexcept
{
  return matrix.lower_triangle_.at(matrix.template lower_triangle_index<Row, Column>());
}

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX_HPP
