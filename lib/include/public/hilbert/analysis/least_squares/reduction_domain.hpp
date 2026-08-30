#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_REDUCTION_DOMAIN_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_REDUCTION_DOMAIN_HPP


#include <concepts>
#include <cstddef>


namespace hilbert::analysis
{

// A reduction domain supplies the observation count used to normalize accumulated products.
template<typename Domain>
concept least_squares_reduction_domain = requires(Domain const &domain) {
  { domain.size() } -> std::same_as<std::size_t>;
};


class fixed_observation_domain
{
  std::size_t size_;

public:
  explicit constexpr fixed_observation_domain(std::size_t size) noexcept;

  [[nodiscard]]
  constexpr std::size_t
  size() const noexcept;
};


class counted_observation_domain
{
  std::size_t count_{};

public:
  void
  observe() noexcept;

  [[nodiscard]]
  std::size_t
  size() const noexcept;
};


constexpr fixed_observation_domain::fixed_observation_domain(std::size_t size) noexcept
    : size_{size}
{
}


constexpr std::size_t
fixed_observation_domain::size() const noexcept
{
  return size_;
}


inline void
counted_observation_domain::observe() noexcept
{
  ++count_;
}


inline std::size_t
counted_observation_domain::size() const noexcept
{
  return count_;
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_REDUCTION_DOMAIN_HPP
