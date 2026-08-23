#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS3_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS3_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix3.hpp>
#include <hilbert/math/linear_algebra/vector3.hpp>

#include <array>
#include <cstddef>


namespace hilbert::analysis
{

template<supported_float Float, std::size_t ResponseCount>
class least_squares_products3
{
  math::symmetric_matrix3<Float> gram_;
  std::array<math::vector3<Float>, ResponseCount> projections_;

public:
  least_squares_products3(
      math::symmetric_matrix3<Float> gram, std::array<math::vector3<Float>, ResponseCount> projections) noexcept
      : gram_{gram}
      , projections_{projections}
  {
  }

  [[nodiscard]]
  math::symmetric_matrix3<Float> const &
  gram() const noexcept
  {
    return gram_;
  }

  template<std::size_t Index>
  requires(Index < ResponseCount)
  [[nodiscard]]
  math::vector3<Float> const &
  projection() const noexcept
  {
    return std::get<Index>(projections_);
  }

  [[nodiscard]]
  std::array<math::vector3<Float>, ResponseCount> const &
  projections() const noexcept
  {
    return projections_;
  }
};

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS3_HPP
