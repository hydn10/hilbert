#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_RESIDUAL_STATISTICS_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_RESIDUAL_STATISTICS_HPP


#include <hilbert/core/supported_float.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>


namespace hilbert::analysis
{

template<supported_float Float, std::size_t ResponseCount>
struct least_squares_residual_statistics
{
  std::array<Float, ResponseCount> response_squared_norms;
  std::size_t observation_count;
};


template<typename Statistics, typename Float, std::size_t ResponseCount>
concept least_squares_residual_statistics_for =
    supported_float<Float> && ResponseCount > 0uz && requires(Statistics const &statistics) {
      { statistics.response_squared_norms } -> std::same_as<std::array<Float, ResponseCount> const &>;
      { statistics.observation_count } -> std::same_as<std::size_t const &>;
    };


template<supported_float Float, std::size_t ResponseCount>
class least_squares_residual_statistics_collector
{
  static_assert(ResponseCount > 0uz);

  std::array<Float, ResponseCount> response_squared_norms_{};

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
  ((std::get<Indices>(response_squared_norms_) += std::get<Indices>(responses) * std::get<Indices>(responses)), ...);
}


template<supported_float Float, std::size_t ResponseCount>
void
least_squares_residual_statistics_collector<Float, ResponseCount>::observe(
    std::array<Float, ResponseCount> const &responses) noexcept
{
  observe_impl(responses, std::make_index_sequence<ResponseCount>{});
}


template<supported_float Float, std::size_t ResponseCount>
least_squares_residual_statistics_collector<Float, ResponseCount>::statistics_type
least_squares_residual_statistics_collector<Float, ResponseCount>::finish(std::size_t observation_count) && noexcept
{
  return {
      .response_squared_norms = std::move(response_squared_norms_),
      .observation_count = observation_count,
  };
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_RESIDUAL_STATISTICS_HPP
