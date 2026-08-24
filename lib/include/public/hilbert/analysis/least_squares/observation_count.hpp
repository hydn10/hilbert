#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_OBSERVATION_COUNT_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_OBSERVATION_COUNT_HPP


#include <cstddef>


namespace hilbert::analysis
{

class exact_observation_count
{
  std::size_t value_;

public:
  explicit constexpr exact_observation_count(std::size_t value) noexcept;

  [[nodiscard]]
  constexpr std::size_t
  value() const noexcept;
};


constexpr exact_observation_count::exact_observation_count(std::size_t value) noexcept
    : value_{value}
{
}


constexpr std::size_t
exact_observation_count::value() const noexcept
{
  return value_;
}


struct count_observations_t
{
};


inline constexpr count_observations_t count_observations;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_OBSERVATION_COUNT_HPP
