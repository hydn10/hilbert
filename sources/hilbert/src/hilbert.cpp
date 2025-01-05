#include <hilbert/hilbert.hpp>

#include <fftw3.h>

#include <complex>
#include <numbers>
#include <ranges>
#include <vector>


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

  auto const output =
      std::views::iota(0, input_size) |
      std::views::transform([&](int idx)
                            { return std::complex<double>(in[idx][0] / input_size, in[idx][1] / input_size); });

  auto res = std::vector(output.begin(), output.end());

  fftw_destroy_plan(forward_plan);
  fftw_destroy_plan(backward_plan);
  fftw_free(in);
  fftw_free(out);

  return res;
}


void
inst_amp_phase_freq(
    std::vector<double> const &data,
    std::vector<double> &amp,
    std::vector<double> &phase,
    std::vector<double> &freq,
    double sampling_rate)
{
  auto const num_samples = data.size();
  auto analytic_signal = hilbert_transform(data);

  phase.resize(num_samples);
  amp.resize(num_samples);
  freq.resize(num_samples);

  for (int i = 0; i < num_samples; ++i)
  {
    amp[i] = std::abs(analytic_signal[i]);
    phase[i] = std::atan2(std::imag(analytic_signal[i]), std::real(analytic_signal[i]));
  }

  double constexpr tau = 2 * std::numbers::pi;

  for (size_t i = 1; i < num_samples; ++i)
  {
    double const delta_phase = phase[i] - phase[i - 1];
    double f = delta_phase * sampling_rate / tau;

    if (f < 0)
    {
      f += sampling_rate;
    }
    freq[i] = f;
  }

  freq[0] = freq[1];
}

} // namespace hilbert
