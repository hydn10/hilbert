#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_NORMAL_EQUATIONS3_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_NORMAL_EQUATIONS3_HPP


#include <hilbert/analysis/least_squares/basis.hpp>
#include <hilbert/analysis/least_squares/detail/discrete_reduction.hpp>
#include <hilbert/analysis/least_squares/observation.hpp>
#include <hilbert/analysis/least_squares/observation_count.hpp>
#include <hilbert/analysis/least_squares/products.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/symmetric_matrix.hpp>
#include <hilbert/math/linear_algebra/vector.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>


namespace hilbert::analysis
{

template<supported_float Float, typename Basis, std::size_t ResponseCount, typename Domain>
class normal_equations_reducer3
{
  static_assert(ResponseCount > 0uz);

  using row_type =
      std::remove_cvref_t<decltype(std::declval<Basis const &>().template row_at<Float>(std::declval<Float>()))>;
  static_assert(row_type::size == 3uz);

  using first_type = row_type::template value_type<0>;
  using second_type = row_type::template value_type<1>;
  using third_type = row_type::template value_type<2>;

  Basis basis_;
  Domain domain_;

  detail::inner_product_term<Float, first_type, first_type> m00_;
  detail::inner_product_term<Float, second_type, first_type> m10_;
  detail::inner_product_term<Float, second_type, second_type> m11_;
  detail::inner_product_term<Float, third_type, first_type> m20_;
  detail::inner_product_term<Float, third_type, second_type> m21_;
  detail::inner_product_term<Float, third_type, third_type> m22_;

  std::array<detail::inner_product_term<Float, first_type, Float>, ResponseCount> first_projections_;
  std::array<detail::inner_product_term<Float, second_type, Float>, ResponseCount> second_projections_;
  std::array<detail::inner_product_term<Float, third_type, Float>, ResponseCount> third_projections_;

public:
  using float_type = Float;
  static constexpr std::size_t response_count = ResponseCount;

  normal_equations_reducer3(Basis basis, Domain domain)
      : basis_{std::move(basis)}
      , domain_{std::move(domain)}
  {
  }

  void
  accumulate(least_squares_observation<Float, ResponseCount> const &observation)
  {
    auto const row = basis_.template row_at<Float>(observation.argument());

    detail::observe_product(m00_, get<0>(row), get<0>(row));
    detail::observe_product(m10_, get<1>(row), get<0>(row));
    detail::observe_product(m11_, get<1>(row), get<1>(row));
    detail::observe_product(m20_, get<2>(row), get<0>(row));
    detail::observe_product(m21_, get<2>(row), get<1>(row));
    detail::observe_product(m22_, get<2>(row), get<2>(row));

    for (
        auto &&[first_projection, second_projection, third_projection, response] :
        std::views::zip(first_projections_, second_projections_, third_projections_, observation.responses()))
    {
      detail::observe_product(first_projection, get<0>(row), response);
      detail::observe_product(second_projection, get<1>(row), response);
      detail::observe_product(third_projection, get<2>(row), response);
    }

    detail::observe_domain(domain_);
  }

  [[nodiscard]]
  least_squares_products<Float, typename Basis::signature_type, ResponseCount>
  finish() &&
  {
    if (domain_.size() < 3uz)
    {
      throw std::invalid_argument{"least-squares fit requires at least three samples"};
    }

    auto const gram = math::symmetric_matrix<
        Float,
        math::dual_signature_t<typename Basis::signature_type>,
        typename Basis::signature_type>::
        from_lower_triangle(
            detail::resolve_reduction<Float>(m00_.finish(), domain_),
            detail::resolve_reduction<Float>(m10_.finish(), domain_),
            detail::resolve_reduction<Float>(m11_.finish(), domain_),
            detail::resolve_reduction<Float>(m20_.finish(), domain_),
            detail::resolve_reduction<Float>(m21_.finish(), domain_),
            detail::resolve_reduction<Float>(m22_.finish(), domain_));

    std::array<math::vector<Float, math::dual_signature_t<typename Basis::signature_type>>, ResponseCount> projections;
    for (
        auto &&[projection, first_projection, second_projection, third_projection] :
        std::views::zip(projections, first_projections_, second_projections_, third_projections_))
    {
      projection = math::vector<Float, math::dual_signature_t<typename Basis::signature_type>>{
          detail::resolve_reduction<Float>(first_projection.finish(), domain_),
          detail::resolve_reduction<Float>(second_projection.finish(), domain_),
          detail::resolve_reduction<Float>(third_projection.finish(), domain_),
      };
    }

    return {gram, projections};
  }
};


template<supported_float Float, typename Basis, std::size_t ResponseCount, typename Domain>
using normal_equations_reducer = normal_equations_reducer3<Float, Basis, ResponseCount, Domain>;


template<supported_float Float, std::size_t ResponseCount, typename Basis>
[[nodiscard]]
auto
make_normal_equations_reducer(Basis basis, exact_observation_count count)
{
  return normal_equations_reducer3<Float, Basis, ResponseCount, detail::known_sample_domain>{
      std::move(basis), detail::known_sample_domain{count.value()}};
}


template<supported_float Float, std::size_t ResponseCount, typename Basis>
[[nodiscard]]
auto
make_normal_equations_reducer(Basis basis, [[maybe_unused]] count_observations_t count)
{
  return normal_equations_reducer3<Float, Basis, ResponseCount, detail::counted_sample_domain>{
      std::move(basis), detail::counted_sample_domain{}};
}


template<supported_float Float, typename Basis, std::ranges::input_range Observations>
requires requires(std::ranges::range_value_t<Observations> const &observation) {
  std::remove_cvref_t<std::ranges::range_value_t<Observations>>::response_count;
  observation.argument();
  observation.responses();
}
[[nodiscard]]
auto
form_normal_equations(Basis basis, Observations &&observations)
{
  auto &&source = std::forward<Observations>(observations);
  using observation_type = std::remove_cvref_t<std::ranges::range_value_t<Observations>>;

  if constexpr (std::ranges::sized_range<Observations>)
  {
    auto reducer = make_normal_equations_reducer<Float, observation_type::response_count>(
        std::move(basis), exact_observation_count{std::ranges::size(source)});

    for (auto &&observation : source)
    {
      reducer.accumulate(observation);
    }

    return std::move(reducer).finish();
  }
  else
  {
    auto reducer =
        make_normal_equations_reducer<Float, observation_type::response_count>(std::move(basis), count_observations);

    for (auto &&observation : source)
    {
      reducer.accumulate(observation);
    }

    return std::move(reducer).finish();
  }
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_NORMAL_EQUATIONS3_HPP
