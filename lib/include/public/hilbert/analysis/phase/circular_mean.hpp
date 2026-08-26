#ifndef HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_HPP
#define HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_HPP


#include <hilbert/analysis/phase/circular_mean_result.hpp>
#include <hilbert/core/supported_float.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>


namespace hilbert::analysis
{

template<supported_float Float>
class circular_mean
{
  std::complex<Float> sum_{};
  std::size_t count_ = 0uz;

public:
  void
  add_vector(std::complex<Float> value);

  [[nodiscard]]
  circular_mean_result<Float>
  finish() const;
};


template<supported_float Float>
void
circular_mean<Float>::add_vector(std::complex<Float> value)
{
  auto const magnitude = std::abs(value);

  if (magnitude == 0)
  {
    throw std::invalid_argument{"circular mean received a zero vector"};
  }

  sum_ += value / magnitude;
  ++count_;
}


template<supported_float Float>
circular_mean_result<Float>
circular_mean<Float>::finish() const
{
  if (count_ == 0uz)
  {
    throw std::invalid_argument{"circular mean requires at least one vector"};
  }

  auto const magnitude = std::abs(sum_);
  if (!std::isfinite(magnitude) || magnitude == 0)
  {
    throw std::invalid_argument{"circular mean has no defined direction"};
  }

  auto const resultant = std::min(static_cast<Float>(1), magnitude / static_cast<Float>(count_));
  return {
      principal_phase<Float>{std::arg(sum_)},
      mean_resultant_length<Float>{resultant},
  };
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_HPP
