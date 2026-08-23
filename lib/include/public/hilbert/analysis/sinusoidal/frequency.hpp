#ifndef HILBERT_ANALYSIS_SINUSOIDAL_FREQUENCY_HPP
#define HILBERT_ANALYSIS_SINUSOIDAL_FREQUENCY_HPP


#include <hilbert/core/supported_float.hpp>

#include <cmath>
#include <numbers>
#include <stdexcept>


namespace hilbert::analysis
{

template<supported_float Float>
class frequency_hz
{
  Float value_;

public:
  explicit frequency_hz(Float value)
      : value_{value}
  {
    if (!std::isfinite(value_) || value_ <= 0)
    {
      throw std::invalid_argument{"frequency must be finite and positive"};
    }
  }

  [[nodiscard]]
  Float
  value() const noexcept
  {
    return value_;
  }

  [[nodiscard]]
  Float
  angular_frequency() const noexcept
  {
    return static_cast<Float>(2) * std::numbers::pi_v<Float> * value_;
  }
};

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SINUSOIDAL_FREQUENCY_HPP
