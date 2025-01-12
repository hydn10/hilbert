#include <hilbert/fft.hpp>


namespace hilbert::fft
{

// According to FFTW documentation ([1]) it is OK to use std::vector<std::complex>> and use reinterpret_cast to
// fftw_complex*.
// [1]: https://www.fftw.org/doc/Complex-numbers.html


plan_r2c::plan_r2c(std::vector<double> const &in, std::vector<std::complex<double>> &out)
    : plan_{fftw_plan_dft_r2c_1d(
          static_cast<int>(in.size()),
          const_cast<double *>(in.data()),
          reinterpret_cast<fftw_complex *>(out.data()),
          FFTW_ESTIMATE)}
{
}


plan_r2c::~plan_r2c()
{
  fftw_destroy_plan(plan_);
}


void
plan_r2c::execute() const
{
  fftw_execute(plan_);
}


constexpr auto
plan_c2c::to_fftw_sign(sign sign)
{
  return sign == sign::BACKWARD ? FFTW_BACKWARD : FFTW_FORWARD;
}


plan_c2c::plan_c2c(std::vector<std::complex<double>> const &in, std::vector<std::complex<double>> &out, sign sign)
    : plan_{fftw_plan_dft_1d(
          static_cast<int>(in.size()),
          reinterpret_cast<fftw_complex *>(const_cast<std::complex<double> *>(in.data())),
          reinterpret_cast<fftw_complex *>(out.data()),
          to_fftw_sign(sign),
          FFTW_ESTIMATE)}
{
}


plan_c2c::~plan_c2c()
{
  fftw_destroy_plan(plan_);
}


void
plan_c2c::execute() const
{
  fftw_execute(plan_);
}


std::vector<std::complex<double>>
fft_transform(std::vector<double> const &input)
{
  std::vector<std::complex<double>> output(input.size());

  fft::plan_r2c forward_plan(input, output);
  forward_plan.execute();

  return output;
}


std::vector<std::complex<double>>
fft_transform(std::vector<std::complex<double>> const &input, sign sign)
{
  std::vector<std::complex<double>> output(input.size());

  plan_c2c forward_plan(input, output, sign);
  forward_plan.execute();

  return output;
}

} // namespace hilbert::fft