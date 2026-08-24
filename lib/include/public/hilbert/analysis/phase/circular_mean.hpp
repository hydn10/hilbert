#ifndef HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_HPP
#define HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_HPP


#include <hilbert/analysis/phase/relative_phase_estimate.hpp>
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
  add_relative_vector(std::complex<Float> value)
  {
    auto const magnitude = std::abs(value);

    if (magnitude == 0)
    {
      throw std::invalid_argument{"circular mean received a zero vector"};
    }
    
    sum_ += value / magnitude;
    ++count_;
  }

  [[nodiscard]]
  relative_phase_estimate<Float>
  finish() const
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
};

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_HPP
