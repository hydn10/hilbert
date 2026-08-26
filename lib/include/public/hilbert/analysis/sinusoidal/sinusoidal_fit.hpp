#ifndef HILBERT_ANALYSIS_SINUSOIDAL_FIT_HPP
#define HILBERT_ANALYSIS_SINUSOIDAL_FIT_HPP


#include <hilbert/analysis/phase/principal_phase.hpp>
#include <hilbert/analysis/frequency_response.hpp>
#include <hilbert/analysis/sinusoidal/basis.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/vector.hpp>

#include <cmath>
#include <complex>
#include <stdexcept>


namespace hilbert::analysis
{

template<supported_float Float>
class sinusoidal_fit
{
  std::complex<Float> phasor_;
  Float dc_offset_;

  sinusoidal_fit(std::complex<Float> phasor, Float dc_offset);

public:
  template<sinusoidal_coordinate_signature Signature>
  static sinusoidal_fit
  from_coefficients(math::vector<Float, Signature> coefficients);

  [[nodiscard]]
  std::complex<Float>
  phasor() const noexcept;

  [[nodiscard]]
  Float
  cosine_coefficient() const noexcept;

  [[nodiscard]]
  Float
  sine_coefficient() const noexcept;

  [[nodiscard]]
  Float
  dc_offset() const noexcept;

  [[nodiscard]]
  Float
  amplitude() const noexcept;
};


template<supported_float Float>
[[nodiscard]]
frequency_response<Float>
make_frequency_response(sinusoidal_fit<Float> const &output, sinusoidal_fit<Float> const &input);


template<supported_float Float>
sinusoidal_fit<Float>::sinusoidal_fit(std::complex<Float> phasor, Float dc_offset)
    : phasor_{phasor}
    , dc_offset_{dc_offset}
{
}


template<supported_float Float>
template<sinusoidal_coordinate_signature Signature>
sinusoidal_fit<Float>
sinusoidal_fit<Float>::from_coefficients(math::vector<Float, Signature> coefficients)
{
  auto const cosine = math::get<cosine_term>(coefficients);
  auto const sine = math::get<sine_term>(coefficients);
  auto const offset = math::get<constant_term>(coefficients);

  if (!std::isfinite(cosine) || !std::isfinite(sine) || !std::isfinite(offset))
  {
    throw std::invalid_argument{"sinusoidal coefficients must be finite"};
  }
  
  return sinusoidal_fit{{cosine, -sine}, offset};
}


template<supported_float Float>
std::complex<Float>
sinusoidal_fit<Float>::phasor() const noexcept
{
  return phasor_;
}


template<supported_float Float>
Float
sinusoidal_fit<Float>::cosine_coefficient() const noexcept
{
  return phasor_.real();
}


template<supported_float Float>
Float
sinusoidal_fit<Float>::sine_coefficient() const noexcept
{
  return -phasor_.imag();
}


template<supported_float Float>
Float
sinusoidal_fit<Float>::dc_offset() const noexcept
{
  return dc_offset_;
}


template<supported_float Float>
Float
sinusoidal_fit<Float>::amplitude() const noexcept
{
  return std::abs(phasor_);
}


template<supported_float Float>
frequency_response<Float>
make_frequency_response(sinusoidal_fit<Float> const &output, sinusoidal_fit<Float> const &input)
{
  auto const input_phasor = input.phasor();

  if (std::abs(input_phasor) == 0)
  {
    throw std::invalid_argument{"frequency response has zero input phasor"};
  }

  return frequency_response<Float>{output.phasor() / input_phasor};
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SINUSOIDAL_FIT_HPP
