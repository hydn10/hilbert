#include <hilbert/detail/fft.hpp>
#include <hilbert/core/supported_float.hpp>

#include <fftw3.h>

#include <complex>
#include <concepts>
#include <span>
#include <vector>


namespace hilbert::detail::fft
{

namespace
{

template<std::floating_point Float>
struct fftw_api;


template<>
struct fftw_api<float>
{
  using plan_type = fftwf_plan;
  using complex_type = fftwf_complex;

  static plan_type
  plan_r2c(int size, float *input, complex_type *output, unsigned flags)
  {
    return fftwf_plan_dft_r2c_1d(size, input, output, flags);
  }

  static plan_type
  plan_c2c(int size, complex_type *input, complex_type *output, int direction, unsigned flags)
  {
    return fftwf_plan_dft_1d(size, input, output, direction, flags);
  }

  static void
  destroy_plan(plan_type plan)
  {
    fftwf_destroy_plan(plan);
  }

  static void
  execute(plan_type plan)
  {
    fftwf_execute(plan);
  }
};


template<>
struct fftw_api<double>
{
  using plan_type = fftw_plan;
  using complex_type = fftw_complex;

  static plan_type
  plan_r2c(int size, double *input, complex_type *output, unsigned flags)
  {
    return fftw_plan_dft_r2c_1d(size, input, output, flags);
  }

  static plan_type
  plan_c2c(int size, complex_type *input, complex_type *output, int direction, unsigned flags)
  {
    return fftw_plan_dft_1d(size, input, output, direction, flags);
  }

  static void
  destroy_plan(plan_type plan)
  {
    fftw_destroy_plan(plan);
  }

  static void
  execute(plan_type plan)
  {
    fftw_execute(plan);
  }
};


template<supported_float Float>
using fftw_api_for = fftw_api<Float>;


template<supported_float Float>
Float *
as_fftw_input(Float const *input) noexcept
{
  // plan_flags guarantees that FFTW does not modify out-of-place inputs.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  return const_cast<Float *>(input);
}


template<supported_float Float>
fftw_api_for<Float>::complex_type *
as_fftw_input(std::complex<Float> const *input) noexcept
{
  // FFTW explicitly documents std::complex<Float> as compatible storage.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto const *const fftw_input = reinterpret_cast<fftw_api_for<Float>::complex_type const *>(input);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  return const_cast<fftw_api_for<Float>::complex_type *>(fftw_input);
}


template<supported_float Float>
fftw_api_for<Float>::complex_type *
as_fftw_output(std::complex<Float> *output) noexcept
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<fftw_api_for<Float>::complex_type *>(output);
}


inline constexpr unsigned plan_flags = FFTW_ESTIMATE | FFTW_PRESERVE_INPUT;


template<supported_float Float>
class plan_r2c
{
  using api = fftw_api_for<Float>;

  static_assert(sizeof(std::complex<Float>) == sizeof(typename api::complex_type));
  static_assert(alignof(std::complex<Float>) >= alignof(typename api::complex_type));

  api::plan_type plan_;

public:
  plan_r2c(std::span<Float const> in, std::span<std::complex<Float>> out)
      : plan_{api::plan_r2c(
            static_cast<int>(in.size()), as_fftw_input(in.data()), as_fftw_output(out.data()), plan_flags)}
  {
  }

  ~plan_r2c()
  {
    api::destroy_plan(plan_);
  }

  plan_r2c(plan_r2c const &) = delete;
  plan_r2c &
  operator=(plan_r2c const &) = delete;
  plan_r2c(plan_r2c &&) = delete;
  plan_r2c &
  operator=(plan_r2c &&) = delete;

  void
  execute() const
  {
    api::execute(plan_);
  }
};


template<supported_float Float>
class plan_c2c
{
  using api = fftw_api_for<Float>;

  static_assert(sizeof(std::complex<Float>) == sizeof(typename api::complex_type));
  static_assert(alignof(std::complex<Float>) >= alignof(typename api::complex_type));

  api::plan_type plan_;

  static constexpr int
  to_fftw_sign(sign direction)
  {
    return direction == sign::backward ? FFTW_BACKWARD : FFTW_FORWARD;
  }

public:
  plan_c2c(std::span<std::complex<Float> const> in, std::span<std::complex<Float>> out, sign direction)
      : plan_{api::plan_c2c(
            static_cast<int>(in.size()),
            as_fftw_input(in.data()),
            as_fftw_output(out.data()),
            to_fftw_sign(direction),
            plan_flags)}
  {
  }

  ~plan_c2c()
  {
    api::destroy_plan(plan_);
  }

  plan_c2c(plan_c2c const &) = delete;
  plan_c2c &
  operator=(plan_c2c const &) = delete;
  plan_c2c(plan_c2c &&) = delete;
  plan_c2c &
  operator=(plan_c2c &&) = delete;

  void
  execute() const
  {
    api::execute(plan_);
  }
};

} // namespace


template<supported_float Float>
std::vector<std::complex<Float>>
transform(std::span<Float const> input)
{
  std::vector<std::complex<Float>> output(input.size());

  plan_r2c<Float> const forward_plan(input, std::span<std::complex<Float>>{output});
  forward_plan.execute();

  return output;
}


template<supported_float Float>
std::vector<std::complex<Float>>
transform(std::span<std::complex<Float> const> input, sign direction)
{
  std::vector<std::complex<Float>> output(input.size());

  plan_c2c<Float> const plan(input, std::span<std::complex<Float>>{output}, direction);
  plan.execute();

  return output;
}


template std::vector<std::complex<double>>
transform<double>(std::span<double const> input);

template std::vector<std::complex<double>>
transform<double>(std::span<std::complex<double> const> input, sign direction);

} // namespace hilbert::detail::fft
