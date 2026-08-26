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


namespace hilbert::analysis
{

namespace detail
{

template<supported_float Float, math::signature_for_size<3uz> Signature>
[[nodiscard]]
auto
column_normalized_gram_matrix(math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram);

} // namespace detail


template<supported_float Float, math::signature_for_size<3uz> Signature>
[[nodiscard]]
Float
column_normalized_basis_condition_number(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram);


template<supported_float Float, math::signature_type Signature>
[[nodiscard]]
Float
normalized_least_squares_residual(
    math::vector<Float, math::dual_signature_t<Signature>> const &projection,
    math::vector<Float, Signature> const &coefficients,
    Float response_squared_norm,
    Float response_sum,
    std::size_t observation_count);


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
      math::get<1, 0>(gram) / std::sqrt(g11 * g00),
      static_cast<Float>(1),
      math::get<2, 0>(gram) / std::sqrt(g22 * g00),
      math::get<2, 1>(gram) / std::sqrt(g22 * g11),
      static_cast<Float>(1));
}


} // namespace detail


template<supported_float Float, math::signature_for_size<3uz> Signature>
Float
column_normalized_basis_condition_number(
    math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> const &gram)
{
  auto const normalized_gram = detail::column_normalized_gram_matrix(gram);
  auto const eigenvalues = math::symmetric_eigenvalues(normalized_gram);
  auto const maximum_eigenvalue = eigenvalues.back();
  auto const minimum_eigenvalue =
      std::max(eigenvalues.front(), std::numeric_limits<Float>::epsilon() * maximum_eigenvalue);

  return std::sqrt(maximum_eigenvalue / minimum_eigenvalue);
}


template<supported_float Float, math::signature_type Signature>
Float
normalized_least_squares_residual(
    math::vector<Float, math::dual_signature_t<Signature>> const &projection,
    math::vector<Float, Signature> const &coefficients,
    Float response_squared_norm,
    Float response_sum,
    std::size_t observation_count)
{
  auto const explained_sum_of_squares = math::dual_pairing(projection, coefficients);

  auto const residual_sum_of_squares =
      std::max(static_cast<Float>(0), response_squared_norm - explained_sum_of_squares);
  auto const count = static_cast<Float>(observation_count);
  auto const total_sum_of_squares =
      std::max(static_cast<Float>(0), response_squared_norm - response_sum * response_sum / count);

  if (total_sum_of_squares == 0)
  {
    return residual_sum_of_squares == 0 ? static_cast<Float>(0) : static_cast<Float>(1);
  }

  return std::sqrt(residual_sum_of_squares / total_sum_of_squares);
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_DIAGNOSTICS_HPP
