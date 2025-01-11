#include <hilbert/fft.hpp>


namespace hilbert::fft
{

void
complex_vec::free_adapter(fftw_complex *p)
{
  fftw_free(p);
}


complex_vec::complex_vec(size_t size)
    : ptr_{fftw_alloc_complex(size), free_adapter}
    , size_{size}
{
}


plan_r2c::plan_r2c(std::vector<double> const &in, complex_vec &out)
    : plan_{
          fftw_plan_dft_r2c_1d(static_cast<int>(in.size()), const_cast<double *>(in.data()), out.data(), FFTW_ESTIMATE)}
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


plan_c2c::plan_c2c(complex_vec const &in, complex_vec &out, sign sign)
    : plan_{fftw_plan_dft_1d(
          static_cast<int>(in.size()),
          const_cast<fftw_complex *>(in.data()),
          out.data(),
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


complex_vec
fft_transform(std::vector<double> const &input)
{
  complex_vec output(input.size());

  fft::plan_r2c forward_plan(input, output);
  forward_plan.execute();

  return output;
}


complex_vec
fft_transform(fft::complex_vec const &input, sign sign)
{
  complex_vec output(input.size());

  plan_c2c forward_plan(input, output, sign);
  forward_plan.execute();

  return output;
}

} // namespace hilbert::fft