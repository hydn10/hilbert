#ifndef HILBERT_ANALYSIS_SINUSOIDAL_FIT_HPP
#define HILBERT_ANALYSIS_SINUSOIDAL_FIT_HPP


#include <hilbert/analysis/phase/principal_phase.hpp>
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

  sinusoidal_fit(std::complex<Float> phasor, Float dc_offset)
      : phasor_{phasor}
      , dc_offset_{dc_offset}
  {
  }

public:
  template<sinusoidal_coordinate_signature Signature>
  static sinusoidal_fit
  from_coefficients(math::vector<Float, Signature> coefficients)
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

  [[nodiscard]]
  std::complex<Float>
  phasor() const noexcept
  {
    return phasor_;
  }

  [[nodiscard]]
  Float
  cosine_coefficient() const noexcept
  {
    return phasor_.real();
  }

  [[nodiscard]]
  Float
  sine_coefficient() const noexcept
  {
    return -phasor_.imag();
  }

  [[nodiscard]]
  Float
  dc_offset() const noexcept
  {
    return dc_offset_;
  }

  [[nodiscard]]
  Float
  amplitude() const noexcept
  {
    return std::abs(phasor_);
  }
};


template<supported_float Float>
[[nodiscard]]
principal_phase<Float>
relative_phase(sinusoidal_fit<Float> const &first, sinusoidal_fit<Float> const &second)
{
  auto const relative_phasor = first.phasor() * std::conj(second.phasor());
  auto const magnitude = std::abs(relative_phasor);

  if (!std::isfinite(magnitude) || magnitude == 0)
  {
    throw std::invalid_argument{"sinusoidal phase is undefined for a zero signal"};
  }

  return principal_phase<Float>{std::arg(relative_phasor)};
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SINUSOIDAL_FIT_HPP
