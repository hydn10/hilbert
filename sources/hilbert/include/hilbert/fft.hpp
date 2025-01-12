#ifndef HILBERT_FFT_HPP
#define HILBERT_FFT_HPP


#include <fftw3.h>

#include <complex>
#include <memory>
#include <span>
#include <vector>


namespace hilbert::fft
{

class plan_r2c
{
  fftw_plan plan_;

public:
  plan_r2c(std::vector<double> const &in, std::vector<std::complex<double>> &out);
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
  FORWARD,
  BACKWARD
};


class plan_c2c
{
  static constexpr auto
  to_fftw_sign(sign sign);

  fftw_plan plan_;

public:
  plan_c2c(std::vector<std::complex<double>> const &in, std::vector<std::complex<double>> &out, sign sign);
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
fft_transform(std::vector<double> const &input);

std::vector<std::complex<double>>
fft_transform(std::vector<std::complex<double>> const &input, sign sign);

} // namespace hilbert::fft

#endif
