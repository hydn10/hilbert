#ifndef HILBERT_FFT_HPP
#define HILBERT_FFT_HPP


#include <fftw3.h>

#include <complex>
#include <memory>
#include <span>
#include <vector>


namespace hilbert::fft
{

class complex_vec
{
  static void
  free_adapter(fftw_complex *p);

  std::unique_ptr<fftw_complex[], decltype(&free_adapter)> ptr_;
  size_t size_;

public:
  explicit complex_vec(size_t size);

  complex_vec(complex_vec const &) = delete;
  complex_vec &
  operator=(complex_vec const &) = delete;
  complex_vec(complex_vec &&) = default;
  complex_vec &
  operator=(complex_vec &&) = default;

  constexpr size_t
  size() const noexcept;

  constexpr fftw_complex *
  data() noexcept;
  constexpr fftw_complex const *
  data() const noexcept;

  constexpr fftw_complex *
  begin() noexcept;
  constexpr fftw_complex const *
  begin() const noexcept;

  constexpr fftw_complex *
  end() noexcept;
  constexpr fftw_complex const *
  end() const noexcept;

  constexpr fftw_complex &
  operator[](size_t pos);
  constexpr fftw_complex const &
  operator[](size_t pos) const;
};


inline constexpr size_t
complex_vec::size() const noexcept
{
  return size_;
}


inline constexpr fftw_complex *
complex_vec::data() noexcept
{
  return ptr_.get();
}


inline constexpr fftw_complex const *
complex_vec::data() const noexcept
{
  return ptr_.get();
}


inline constexpr fftw_complex *
complex_vec::begin() noexcept
{
  return ptr_.get();
}


inline constexpr fftw_complex const *
complex_vec::begin() const noexcept
{
  return ptr_.get();
}


inline constexpr fftw_complex *
complex_vec::end() noexcept
{
  return ptr_.get() + size_;
}


inline constexpr fftw_complex const *
complex_vec::end() const noexcept
{
  return ptr_.get() + size_;
}


inline constexpr fftw_complex &
complex_vec::operator[](size_t pos)
{
  return ptr_[pos];
}


inline constexpr fftw_complex const &
complex_vec::operator[](size_t pos) const
{
  return ptr_[pos];
}


class plan_r2c
{
  fftw_plan plan_;

public:
  plan_r2c(std::vector<double> const &in, complex_vec &out);
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
  plan_c2c(complex_vec const &in, complex_vec &out, sign sign);
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


complex_vec
fft_transform(std::vector<double> const &input);

complex_vec
fft_transform(fft::complex_vec const &input, sign sign);

} // namespace hilbert::fft

#endif
