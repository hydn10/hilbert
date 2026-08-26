#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP


#include <hilbert/core/supported_float.hpp>
#include <hilbert/detail/attributes.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix.hpp>
#include <hilbert/math/linear_algebra/vector.hpp>

#include <array>
#include <cstddef>
#include <utility>


namespace hilbert::analysis
{

struct no_least_squares_statistics
{
};


template<
    supported_float Float,
    math::signature_type Signature,
    std::size_t ResponseCount,
    typename Statistics = no_least_squares_statistics>
class least_squares_products
{
  static_assert(ResponseCount > 0uz);

public:
  using signature_type = Signature;
  using projection_type = math::vector<Float, math::dual_signature_t<Signature>>;
  using gram_type = math::symmetric_matrix<Float, math::dual_signature_t<Signature>, Signature>;
  using statistics_type = Statistics;

private:
  gram_type gram_;
  std::array<projection_type, ResponseCount> projections_;

  HILBERT_NO_UNIQUE_ADDRESS
  Statistics statistics_;

public:
  least_squares_products(
      gram_type gram, std::array<projection_type, ResponseCount> projections, Statistics statistics = {}) noexcept;

  [[nodiscard]]
  gram_type const &
  gram() const noexcept;

  template<std::size_t Index>
  requires(Index < ResponseCount)
  [[nodiscard]]
  projection_type const &
  projection() const noexcept;

  [[nodiscard]]
  std::array<projection_type, ResponseCount> const &
  projections() const noexcept;

  [[nodiscard]]
  Statistics const &
  statistics() const noexcept;
};


template<supported_float Float, math::signature_type Signature, std::size_t ResponseCount, typename Statistics>
least_squares_products<Float, Signature, ResponseCount, Statistics>::least_squares_products(
    gram_type gram, std::array<projection_type, ResponseCount> projections, Statistics statistics) noexcept
    : gram_{gram}
    , projections_{projections}
    , statistics_{std::move(statistics)}
{
}


template<supported_float Float, math::signature_type Signature, std::size_t ResponseCount, typename Statistics>
least_squares_products<Float, Signature, ResponseCount, Statistics>::gram_type const &
least_squares_products<Float, Signature, ResponseCount, Statistics>::gram() const noexcept
{
  return gram_;
}


template<supported_float Float, math::signature_type Signature, std::size_t ResponseCount, typename Statistics>
template<std::size_t Index>
requires(Index < ResponseCount)
least_squares_products<Float, Signature, ResponseCount, Statistics>::projection_type const &
least_squares_products<Float, Signature, ResponseCount, Statistics>::projection() const noexcept
{
  return std::get<Index>(projections_);
}


template<supported_float Float, math::signature_type Signature, std::size_t ResponseCount, typename Statistics>
std::array<
    typename least_squares_products<Float, Signature, ResponseCount, Statistics>::projection_type,
    ResponseCount> const &
least_squares_products<Float, Signature, ResponseCount, Statistics>::projections() const noexcept
{
  return projections_;
}


template<supported_float Float, math::signature_type Signature, std::size_t ResponseCount, typename Statistics>
Statistics const &
least_squares_products<Float, Signature, ResponseCount, Statistics>::statistics() const noexcept
{
  return statistics_;
}


template<
    supported_float Float,
    math::signature_for_size<3uz> Signature,
    std::size_t ResponseCount,
    typename Statistics = no_least_squares_statistics>
using least_squares_products3 = least_squares_products<Float, Signature, ResponseCount, Statistics>;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS_HPP
