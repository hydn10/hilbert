#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix.hpp>
#include <hilbert/math/linear_algebra/vector.hpp>

#include <array>
#include <cstddef>


namespace hilbert::analysis
{

template<supported_float Float, std::size_t BasisSize, std::size_t ResponseCount>
class least_squares_products
{
  math::symmetric_matrix<Float, BasisSize> gram_;
  std::array<math::vector<Float, BasisSize>, ResponseCount> projections_;

public:
  least_squares_products(
      math::symmetric_matrix<Float, BasisSize> gram,
      std::array<math::vector<Float, BasisSize>, ResponseCount> projections) noexcept
      : gram_{gram}
      , projections_{projections}
  {
  }

  [[nodiscard]]
  math::symmetric_matrix<Float, BasisSize> const &
  gram() const noexcept
  {
    return gram_;
  }

  template<std::size_t Index>
  requires(Index < ResponseCount)
  [[nodiscard]]
  math::vector<Float, BasisSize> const &
  projection() const noexcept
  {
    return std::get<Index>(projections_);
  }

  [[nodiscard]]
  std::array<math::vector<Float, BasisSize>, ResponseCount> const &
  projections() const noexcept
  {
    return projections_;
  }
};

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP
