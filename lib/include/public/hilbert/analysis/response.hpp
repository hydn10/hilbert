#ifndef HILBERT_ANALYSIS_RESPONSE_HPP
#define HILBERT_ANALYSIS_RESPONSE_HPP


#include <hilbert/analysis/phase/principal_phase.hpp>
#include <hilbert/core/supported_float.hpp>

#include <complex>
#include <stdexcept>


namespace hilbert::analysis
{

template<supported_float Float>
class frequency_response
{
  std::complex<Float> value_;

public:
  explicit frequency_response(std::complex<Float> value) noexcept;

  [[nodiscard]]
  std::complex<Float>
  value() const noexcept;

  [[nodiscard]]
  Float
  magnitude() const noexcept;

  [[nodiscard]]
  principal_phase<Float>
  phase() const;
};


template<supported_float Float>
frequency_response<Float>::frequency_response(std::complex<Float> value) noexcept
    : value_{value}
{
}


template<supported_float Float>
std::complex<Float>
frequency_response<Float>::value() const noexcept
{
  return value_;
}


template<supported_float Float>
Float
frequency_response<Float>::magnitude() const noexcept
{
  return std::abs(value_);
}


template<supported_float Float>
principal_phase<Float>
frequency_response<Float>::phase() const
{
  if (magnitude() == 0)
  {
    throw std::invalid_argument{"frequency-response phase is undefined for a zero response"};
  }

  return principal_phase<Float>{std::arg(value_)};
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_RESPONSE_HPP
