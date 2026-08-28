#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_DIAGNOSTICS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_DIAGNOSTICS_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/signature.hpp>
#include <hilbert/math/linear_algebra/symmetric_eigenvalues3.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix.hpp>
#include <hilbert/math/linear_algebra/vector.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>


namespace hilbert::analysis
{

namespace detail
{

template<supported_float Float, math::signature_for_size<3uz> Signature>
[[nodiscard]]
auto
column_normalized_gram_matrix(math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram);


template<std::size_t Row, supported_float Float, math::signature_type Signature, std::size_t... Columns>
[[nodiscard]]
Float
matrix_row_product(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &matrix,
    math::vector<Float, Signature> const &vector,
    [[maybe_unused]] std::index_sequence<Columns...> columns);


template<supported_float Float, math::signature_type Signature, std::size_t... Rows>
[[nodiscard]]
Float
quadratic_form(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &matrix,
    math::vector<Float, Signature> const &vector,
    [[maybe_unused]] std::index_sequence<Rows...> rows);


template<supported_float Float, math::signature_type Signature, std::size_t... Indices>
[[nodiscard]]
Float
dual_pairing(
    math::vector<Float, math::dual_signature_t<Signature>> const &dual,
    math::vector<Float, Signature> const &primal,
    [[maybe_unused]] std::index_sequence<Indices...> indices);

} // namespace detail


template<supported_float Float, math::signature_for_size<3uz> Signature>
[[nodiscard]]
Float
column_normalized_basis_condition_number(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram);


template<supported_float Float, math::signature_for_size<3uz> Signature>
[[nodiscard]]
Float
column_normalized_basis_reciprocal_condition_number(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram);


template<supported_float Float, math::signature_type Signature>
[[nodiscard]]
Float
normalized_least_squares_residual(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram,
    math::vector<Float, math::dual_signature_t<Signature>> const &projection,
    math::vector<Float, Signature> const &coefficients,
    Float response_squared_norm,
    Float response_centered_squared_norm);


namespace detail
{

template<supported_float Float, math::signature_for_size<3uz> Signature>
auto
column_normalized_gram_matrix(math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram)
{
  auto const g00 = math::get<0, 0>(gram);
  auto const g11 = math::get<1, 1>(gram);
  auto const g22 = math::get<2, 2>(gram);

  return math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature>::from_lower_triangle(
      static_cast<Float>(1),
      math::get<1, 0>(gram) / (std::sqrt(g11) * std::sqrt(g00)),
      static_cast<Float>(1),
      math::get<2, 0>(gram) / (std::sqrt(g22) * std::sqrt(g00)),
      math::get<2, 1>(gram) / (std::sqrt(g22) * std::sqrt(g11)),
      static_cast<Float>(1));
}


template<std::size_t Row, supported_float Float, math::signature_type Signature, std::size_t... Columns>
[[nodiscard]]
Float
matrix_row_product(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &matrix,
    math::vector<Float, Signature> const &vector,
    [[maybe_unused]] std::index_sequence<Columns...> columns)
{
  Float product{};
  ((product = std::fma(math::get<Row, Columns>(matrix), math::get<Columns>(vector), product)), ...);
  return product;
}


template<supported_float Float, math::signature_type Signature, std::size_t... Rows>
[[nodiscard]]
Float
quadratic_form(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &matrix,
    math::vector<Float, Signature> const &vector,
    [[maybe_unused]] std::index_sequence<Rows...> rows)
{
  Float result{};
  ((result = std::fma(
        math::get<Rows>(vector),
        matrix_row_product<Rows>(matrix, vector, std::make_index_sequence<Signature::size>{}),
        result)),
   ...);
  return result;
}


template<supported_float Float, math::signature_type Signature, std::size_t... Indices>
[[nodiscard]]
Float
dual_pairing(
    math::vector<Float, math::dual_signature_t<Signature>> const &dual,
    math::vector<Float, Signature> const &primal,
    [[maybe_unused]] std::index_sequence<Indices...> indices)
{
  Float result{};
  ((result = std::fma(math::get<Indices>(dual), math::get<Indices>(primal), result)), ...);
  return result;
}


} // namespace detail


template<supported_float Float, math::signature_for_size<3uz> Signature>
Float
column_normalized_basis_reciprocal_condition_number(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram)
{
  auto const g00 = math::get<0, 0>(gram);
  auto const g11 = math::get<1, 1>(gram);
  auto const g22 = math::get<2, 2>(gram);

  // A zero-norm basis column makes the normalized design matrix singular.
  if (!(g00 > 0) || !(g11 > 0) || !(g22 > 0))
  {
    return static_cast<Float>(0);
  }

  auto const normalized_gram = detail::column_normalized_gram_matrix(gram);
  auto const eigenvalues = math::symmetric_eigenvalues(normalized_gram);

  auto const maximum_eigenvalue = eigenvalues.back();
  auto const minimum_eigenvalue = eigenvalues.front();

  // Reciprocal conditioning represents numerically singular cases explicitly instead of capping them.
  if (maximum_eigenvalue <= 0 || minimum_eigenvalue <= std::numeric_limits<Float>::epsilon() * maximum_eigenvalue)
  {
    return static_cast<Float>(0);
  }

  return std::sqrt(minimum_eigenvalue / maximum_eigenvalue);
}


template<supported_float Float, math::signature_for_size<3uz> Signature>
Float
column_normalized_basis_condition_number(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram)
{
  auto const reciprocal_condition_number = column_normalized_basis_reciprocal_condition_number(gram);

  return reciprocal_condition_number > 0 ? static_cast<Float>(1) / reciprocal_condition_number
                                         : std::numeric_limits<Float>::infinity();
}


template<supported_float Float, math::signature_type Signature>
Float
normalized_least_squares_residual(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram,
    math::vector<Float, math::dual_signature_t<Signature>> const &projection,
    math::vector<Float, Signature> const &coefficients,
    Float response_squared_norm,
    Float response_centered_squared_norm)
{
  auto const projection_term =
      detail::dual_pairing(projection, coefficients, std::make_index_sequence<Signature::size>{});
  auto const coefficient_term = detail::quadratic_form(gram, coefficients, std::make_index_sequence<Signature::size>{});

  // Evaluate the full objective for the coefficients actually produced by the numerical solve.
  auto residual_sum_of_squares = std::fma(static_cast<Float>(-2), projection_term, response_squared_norm);
  residual_sum_of_squares += coefficient_term;
  residual_sum_of_squares = std::max(static_cast<Float>(0), residual_sum_of_squares);
  auto const total_sum_of_squares = std::max(static_cast<Float>(0), response_centered_squared_norm);

  if (total_sum_of_squares == 0)
  {
    return residual_sum_of_squares == 0 ? static_cast<Float>(0) : static_cast<Float>(1);
  }

  return static_cast<Float>(std::sqrt(residual_sum_of_squares / total_sum_of_squares));
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_DIAGNOSTICS_HPP
