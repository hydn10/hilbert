#ifndef HILBERT_ANALYSIS_SINUSOIDAL_BASIS_HPP
#define HILBERT_ANALYSIS_SINUSOIDAL_BASIS_HPP


#include <hilbert/analysis/least_squares/basis.hpp>
#include <hilbert/analysis/sinusoidal/frequency.hpp>
#include <hilbert/core/supported_float.hpp>

#include <cmath>


namespace hilbert::analysis
{

template<supported_float Float>
class cosine_basis_function
{
  Float angular_frequency_;

public:
  explicit constexpr cosine_basis_function(Float angular_frequency) noexcept
      : angular_frequency_{angular_frequency}
  {
  }

  [[nodiscard]]
  Float
  operator()(Float time) const noexcept
  {
    return std::cos(angular_frequency_ * time);
  }
};


template<supported_float Float>
class sine_basis_function
{
  Float angular_frequency_;

public:
  explicit constexpr sine_basis_function(Float angular_frequency) noexcept
      : angular_frequency_{angular_frequency}
  {
  }

  [[nodiscard]]
  Float
  operator()(Float time) const noexcept
  {
    return std::sin(angular_frequency_ * time);
  }
};


template<supported_float Float>
[[nodiscard]]
auto
make_sinusoidal_basis(frequency_hz<Float> const &frequency)
{
  auto const angular_frequency = frequency.angular_frequency();

  return basis{
      cosine_basis_function<Float>{angular_frequency},
      sine_basis_function<Float>{angular_frequency},
      constant_one,
  };
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SINUSOIDAL_BASIS_HPP
