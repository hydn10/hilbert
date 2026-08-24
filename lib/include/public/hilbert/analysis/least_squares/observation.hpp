#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_OBSERVATION_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_OBSERVATION_HPP


#include <hilbert/core/supported_float.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>


namespace hilbert::analysis
{

template<supported_float Float, std::size_t ResponseCount>
class least_squares_observation
{
  Float argument_;
  std::array<Float, ResponseCount> responses_;

public:
  using float_type = Float;
  static constexpr std::size_t response_count = ResponseCount;

  least_squares_observation(Float argument, std::array<Float, ResponseCount> responses) noexcept;

  [[nodiscard]]
  Float
  argument() const noexcept;

  [[nodiscard]]
  std::array<Float, ResponseCount> const &
  responses() const noexcept;
};


template<supported_float Float, typename... Responses>
requires(sizeof...(Responses) > 0uz) && (std::same_as<std::remove_cvref_t<Responses>, Float> && ...)
[[nodiscard]]
auto
make_observation(Float argument, Responses... responses) noexcept;


template<supported_float Float, std::size_t ResponseCount>
least_squares_observation<Float, ResponseCount>::least_squares_observation(
    Float argument, std::array<Float, ResponseCount> responses) noexcept
    : argument_{argument}
    , responses_{responses}
{
}


template<supported_float Float, std::size_t ResponseCount>
Float
least_squares_observation<Float, ResponseCount>::argument() const noexcept
{
  return argument_;
}


template<supported_float Float, std::size_t ResponseCount>
std::array<Float, ResponseCount> const &
least_squares_observation<Float, ResponseCount>::responses() const noexcept
{
  return responses_;
}


template<supported_float Float, typename... Responses>
requires(sizeof...(Responses) > 0uz) && (std::same_as<std::remove_cvref_t<Responses>, Float> && ...)
auto
make_observation(Float argument, Responses... responses) noexcept
{
  return least_squares_observation<Float, sizeof...(Responses)>{
      argument,
      std::array<Float, sizeof...(Responses)>{responses...},
  };
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_OBSERVATION_HPP
