#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix.hpp>
#include <hilbert/math/linear_algebra/vector.hpp>

#include <array>
#include <cstddef>


namespace hilbert::analysis
{

template<supported_float Float, math::signature_type Signature, std::size_t ResponseCount>
class least_squares_products
{
  math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature> gram_;
  std::array<math::vector<Float, math::dual_signature_t<Signature>>, ResponseCount> projections_;

public:
  using signature_type = Signature;
  using projection_type = math::vector<Float, math::dual_signature_t<Signature>>;
  using gram_type = math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature>;

  least_squares_products(
      gram_type gram,
      std::array<projection_type, ResponseCount> projections) noexcept
      : gram_{gram}
      , projections_{projections}
  {
  }

  [[nodiscard]]
  gram_type const &
  gram() const noexcept
  {
    return gram_;
  }

  template<std::size_t Index>
  requires(Index < ResponseCount)
  [[nodiscard]]
  projection_type const &
  projection() const noexcept
  {
    return std::get<Index>(projections_);
  }

  [[nodiscard]]
  std::array<projection_type, ResponseCount> const &
  projections() const noexcept
  {
    return projections_;
  }
};

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP
