#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_RESIDUAL_STATISTICS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_RESIDUAL_STATISTICS_HPP


#include <hilbert/core/supported_float.hpp>

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <utility>


namespace hilbert::analysis
{

template<supported_float Float, std::size_t ResponseCount>
struct least_squares_residual_statistics
{
  using accumulator_type = Float;

  std::array<accumulator_type, ResponseCount> response_squared_norms;
  std::array<accumulator_type, ResponseCount> response_means;
  std::array<accumulator_type, ResponseCount> response_centered_squared_norms;
  std::size_t observation_count;
};


template<typename Statistics, typename Float, std::size_t ResponseCount>
concept least_squares_residual_statistics_for =
    supported_float<Float> && ResponseCount > 0uz && requires(Statistics const &statistics) {
      typename Statistics::accumulator_type;
      {
        statistics.response_squared_norms
      } -> std::same_as<std::array<typename Statistics::accumulator_type, ResponseCount> const &>;
      {
        statistics.response_means
      } -> std::same_as<std::array<typename Statistics::accumulator_type, ResponseCount> const &>;
      {
        statistics.response_centered_squared_norms
      } -> std::same_as<std::array<typename Statistics::accumulator_type, ResponseCount> const &>;
      { statistics.observation_count } -> std::same_as<std::size_t const &>;
    };


template<supported_float Float, std::size_t ResponseCount>
class least_squares_residual_statistics_collector
{
  static_assert(ResponseCount > 0uz);

  using accumulator_type = least_squares_residual_statistics<Float, ResponseCount>::accumulator_type;

  std::array<accumulator_type, ResponseCount> response_squared_norms_{};
  std::array<accumulator_type, ResponseCount> response_means_{};
  std::array<accumulator_type, ResponseCount> response_centered_squared_norms_{};
  std::size_t observation_count_{};

  template<std::size_t... Indices>
  void
  observe_impl(
      std::array<Float, ResponseCount> const &responses,
      [[maybe_unused]] std::index_sequence<Indices...> indices) noexcept;

public:
  using statistics_type = least_squares_residual_statistics<Float, ResponseCount>;

  void
  observe(std::array<Float, ResponseCount> const &responses) noexcept;

  [[nodiscard]]
  statistics_type
  finish(std::size_t observation_count) && noexcept;
};


template<supported_float Float, std::size_t ResponseCount>
template<std::size_t... Indices>
void
least_squares_residual_statistics_collector<Float, ResponseCount>::observe_impl(
    std::array<Float, ResponseCount> const &responses,
    [[maybe_unused]] std::index_sequence<Indices...> indices) noexcept
{
  auto const count = static_cast<accumulator_type>(observation_count_);
  auto const update = [this, &responses, count]<std::size_t Index>()
  {
    auto const response = static_cast<accumulator_type>(std::get<Index>(responses));
    std::get<Index>(response_squared_norms_) = std::fma(response, response, std::get<Index>(response_squared_norms_));

    // Welford avoids cancellation when accumulating centered response energy.
    auto const delta = response - std::get<Index>(response_means_);
    std::get<Index>(response_means_) += delta / count;
    auto const centered_delta = response - std::get<Index>(response_means_);
    std::get<Index>(response_centered_squared_norms_) =
        std::fma(delta, centered_delta, std::get<Index>(response_centered_squared_norms_));
  };

  (update.template operator()<Indices>(), ...);
}


template<supported_float Float, std::size_t ResponseCount>
void
least_squares_residual_statistics_collector<Float, ResponseCount>::observe(
    std::array<Float, ResponseCount> const &responses) noexcept
{
  ++observation_count_;
  observe_impl(responses, std::make_index_sequence<ResponseCount>{});
}


template<supported_float Float, std::size_t ResponseCount>
least_squares_residual_statistics_collector<Float, ResponseCount>::statistics_type
least_squares_residual_statistics_collector<Float, ResponseCount>::finish(std::size_t observation_count) && noexcept
{
  return {
      .response_squared_norms = std::move(response_squared_norms_),
      .response_means = std::move(response_means_),
      .response_centered_squared_norms = std::move(response_centered_squared_norms_),
      .observation_count = observation_count,
  };
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_RESIDUAL_STATISTICS_HPP
