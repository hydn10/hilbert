#include <hilbert/detail/fft.hpp>

#include <fftw3.h>

#include <complex>
#include <span>
#include <vector>


namespace hilbert::detail::fft
{

namespace
{

static_assert(sizeof(std::complex<double>) == sizeof(fftw_complex));
static_assert(alignof(std::complex<double>) >= alignof(fftw_complex));


inline constexpr unsigned plan_flags = FFTW_ESTIMATE | FFTW_PRESERVE_INPUT;


double *
as_fftw_input(double const *input) noexcept
{
  // plan_flags guarantees that FFTW does not modify out-of-place inputs.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  return const_cast<double *>(input);
}


fftw_complex *
as_fftw_input(std::complex<double> const *input) noexcept
{
  // FFTW explicitly documents std::complex<double> as compatible storage:
  // https://fftw.org/fftw3_doc/Complex-numbers.html
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast,cppcoreguidelines-pro-type-reinterpret-cast)
  return const_cast<fftw_complex *>(reinterpret_cast<fftw_complex const *>(input));
}


fftw_complex *
as_fftw_output(std::complex<double> *output) noexcept
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<fftw_complex *>(output);
}

} // namespace


plan_r2c::plan_r2c(std::span<double const> in, std::span<std::complex<double>> out)
    : plan_{fftw_plan_dft_r2c_1d(
          static_cast<int>(in.size()), as_fftw_input(in.data()), as_fftw_output(out.data()), plan_flags)}
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


constexpr int
plan_c2c::to_fftw_sign(sign direction)
{
  return direction == sign::backward ? FFTW_BACKWARD : FFTW_FORWARD;
}


plan_c2c::plan_c2c(std::span<std::complex<double> const> in, std::span<std::complex<double>> out, sign direction)
    : plan_{fftw_plan_dft_1d(
          static_cast<int>(in.size()),
          as_fftw_input(in.data()),
          as_fftw_output(out.data()),
          to_fftw_sign(direction),
          plan_flags)}
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
transform(std::span<double const> input)
{
  std::vector<std::complex<double>> output(input.size());

  plan_r2c const forward_plan(input, output);
  forward_plan.execute();

  return output;
}


std::vector<std::complex<double>>
transform(std::span<std::complex<double> const> input, sign direction)
{
  std::vector<std::complex<double>> output(input.size());

  plan_c2c const plan(input, output, direction);
  plan.execute();

  return output;
}

} // namespace hilbert::detail::fft
