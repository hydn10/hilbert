#include <hilbert/hilbert.hpp>

#include <fftw3.h>

#include <vector>
#include <complex>
#include <numbers>


namespace hilbert
{

void
hilbert_transform(std::vector<double> const &input, std::vector<std::complex<double>> &output)
{
  int N = static_cast<int>(input.size());
  fftw_complex *in, *out;
  fftw_plan p;

  in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
  out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

  for (int i = 0; i < N; ++i)
  {
    in[i][0] = input[i];
    in[i][1] = 0.0;
  }

  p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);

  for (int i = 1; i < N / 2; ++i)
  {
    out[i][0] *= 2;
    out[i][1] *= 2;
  }
  for (int i = N / 2 + 1; i < N; ++i)
  {
    out[i][0] = 0;
    out[i][1] = 0;
  }

  p = fftw_plan_dft_1d(N, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(p);

  for (int i = 0; i < N; ++i)
  {
    output[i] = std::complex<double>(in[i][0] / N, in[i][1] / N);
  }

  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
}

void
inst_freq_amp(std::vector<double> const &data, std::vector<double> &freq, std::vector<double> &amp, double dt)
{
  size_t N = data.size();
  std::vector<std::complex<double>> analyticSignal(N);
  hilbert_transform(data, analyticSignal);

  std::vector<double> phase(N);
  amp.resize(N);
  freq.resize(N);

  for (int i = 0; i < N; ++i)
  {
    amp[i] = std::abs(analyticSignal[i]);
    phase[i] = std::atan2(std::imag(analyticSignal[i]), std::real(analyticSignal[i]));
  }

  for (size_t i = 1; i < N; ++i)
  {
    double deltaPhase = phase[i] - phase[i - 1];
    if (deltaPhase < 0)
    {
      deltaPhase += 2 * std::numbers::pi;
    }
    freq[i] = deltaPhase / (2 * std::numbers::pi * dt);
  }

  freq[0] = freq[1];
}

} // namespace hilbert
