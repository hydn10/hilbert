#ifndef HILBERT_ANALYSIS_PHASE_PRINCIPAL_PHASE_HPP
#define HILBERT_ANALYSIS_PHASE_PRINCIPAL_PHASE_HPP


#include <hilbert/core/supported_float.hpp>

#include <cmath>
#include <numbers>
#include <stdexcept>


namespace hilbert::analysis
{

template<supported_float Float>
class principal_phase
{
  Float radians_;

public:
  explicit principal_phase(Float radians)
      : radians_{radians}
  {
    auto constexpr pi_value = std::numbers::pi_v<Float>;
    if (!std::isfinite(radians_) || radians_ < -pi_value || radians_ > pi_value)
    {
      throw std::invalid_argument{"phase must be a finite principal angle"};
    }
  }

  [[nodiscard]]
  Float
  radians() const noexcept
  {
    return radians_;
  }
};

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_PHASE_PRINCIPAL_PHASE_HPP
