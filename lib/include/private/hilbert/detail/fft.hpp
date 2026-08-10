#ifndef HILBERT_DETAIL_FFT_HPP
#define HILBERT_DETAIL_FFT_HPP


#include <fftw3.h>

#include <complex>
#include <span>
#include <vector>


namespace hilbert::detail::fft
{

class plan_r2c
{
  fftw_plan plan_;

public:
  plan_r2c(std::span<double const> in, std::span<std::complex<double>> out);
  ~plan_r2c();

  plan_r2c(plan_r2c const &) = delete;
  plan_r2c &
  operator=(plan_r2c const &) = delete;
  plan_r2c(plan_r2c &&) = delete;
  plan_r2c &
  operator=(plan_r2c &&) = delete;

  void
  execute() const;
};


enum class sign
{
  forward,
  backward,
};


class plan_c2c
{
  static constexpr int
  to_fftw_sign(sign direction);

  fftw_plan plan_;

public:
  plan_c2c(std::span<std::complex<double> const> in, std::span<std::complex<double>> out, sign direction);
  ~plan_c2c();

  plan_c2c(plan_c2c const &) = delete;
  plan_c2c &
  operator=(plan_c2c const &) = delete;
  plan_c2c(plan_c2c &&) = delete;
  plan_c2c &
  operator=(plan_c2c &&) = delete;

  void
  execute() const;
};


std::vector<std::complex<double>>
transform(std::span<double const> input);


std::vector<std::complex<double>>
transform(std::span<std::complex<double> const> input, sign direction);

} // namespace hilbert::detail::fft

#endif
