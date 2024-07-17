#include <hilbert/hilbert.hpp>

#include <fftw3.h>

#include <vector>
#include <complex>
#include <numbers>
#include <ranges>


namespace hilbert
{

std::vector<std::complex<double>>
hilbert_transform(std::vector<double> const &input)
{
  auto input_size = static_cast<int>(input.size());

  auto *const in = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * input_size));
  auto *const out = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * input_size));

  for (int i = 0; i < input_size; ++i)
  {
    in[i][0] = input[i];
    in[i][1] = 0.0;
  }

  auto const forward_plan = fftw_plan_dft_1d(input_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(forward_plan);

  for (int i = 1; i < input_size / 2; ++i)
  {
    out[i][0] *= 2;
    out[i][1] *= 2;
  }
  for (int i = input_size / 2 + 1; i < input_size; ++i)
  {
    out[i][0] = 0;
    out[i][1] = 0;
  }

  auto const backward_plan = fftw_plan_dft_1d(input_size, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(backward_plan);

  auto const output = std::views::iota(0, input_size)
      | std::views::transform([&](int idx)
      {
        return std::complex<double>(in[idx][0] / input_size, in[idx][1] / input_size);
      });

  auto res = std::vector(output.begin(), output.end());

  fftw_destroy_plan(forward_plan);
  fftw_destroy_plan(backward_plan);
  fftw_free(in);
  fftw_free(out);

  return res;
}


void
inst_freq_amp(std::vector<double> const &data, std::vector<double> &freq, std::vector<double> &amp, double dt)
{
  size_t const data_size = data.size();
  auto analytic_signal = hilbert_transform(data);

  std::vector<double> phase(data_size);
  amp.resize(data_size);
  freq.resize(data_size);

  for (int i = 0; i < data_size; ++i)
  {
    amp[i] = std::abs(analytic_signal[i]);
    phase[i] = std::atan2(std::imag(analytic_signal[i]), std::real(analytic_signal[i]));
  }

  for (size_t i = 1; i < data_size; ++i)
  {
    double delta_phase = phase[i] - phase[i - 1];
    if (delta_phase < 0)
    {
      delta_phase += 2 * std::numbers::pi;
    }
    freq[i] = delta_phase / (2 * std::numbers::pi * dt);
  }

  freq[0] = freq[1];
}

} // namespace hilbert
