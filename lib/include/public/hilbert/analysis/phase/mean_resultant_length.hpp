#ifndef HILBERT_ANALYSIS_PHASE_MEAN_RESULTANT_LENGTH_HPP
#define HILBERT_ANALYSIS_PHASE_MEAN_RESULTANT_LENGTH_HPP


#include <hilbert/core/supported_float.hpp>

#include <cmath>
#include <stdexcept>


namespace hilbert::analysis
{

template<supported_float Float>
class mean_resultant_length
{
  Float value_;

public:
  explicit mean_resultant_length(Float value)
      : value_{value}
  {
    if (!std::isfinite(value_) || value_ < 0 || value_ > 1)
    {
      throw std::invalid_argument{"mean resultant length must be in [0, 1]"};
    }
  }

  [[nodiscard]]
  Float
  value() const noexcept
  {
    return value_;
  }
};

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_PHASE_MEAN_RESULTANT_LENGTH_HPP
