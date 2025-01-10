#include <hilbert/hilbert.hpp>

#include <fftw3.h>

#include <complex>
#include <memory>
#include <numbers>
#include <ranges>
#include <vector>


namespace hilbert
{

namespace
{

void
fftw_free_adapter(fftw_complex *ptr)
{
  fftw_free(ptr);
}


std::unique_ptr<fftw_complex[], decltype(&fftw_free_adapter)>
make_fftw_vector(size_t size)
{
  auto *const ptr = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * size));
  return {ptr, fftw_free_adapter};
}

} // namespace


std::vector<std::complex<double>>
hilbert_transform(std::vector<double> const &input)
{
  auto input_size = static_cast<int>(input.size());

  auto in = make_fftw_vector(input_size);
  auto out = make_fftw_vector(input_size);

  for (int i = 0; i < input_size; ++i)
  {
    in[i][0] = input[i];
    in[i][1] = 0.0;
  }

  auto const forward_plan = fftw_plan_dft_1d(input_size, in.get(), out.get(), FFTW_FORWARD, FFTW_ESTIMATE);
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

  auto const backward_plan = fftw_plan_dft_1d(input_size, out.get(), in.get(), FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(backward_plan);

  auto const output =
      std::views::iota(0, input_size) |
      std::views::transform([&](int idx)
                            { return std::complex<double>(in[idx][0] / input_size, in[idx][1] / input_size); });

  auto res = std::vector(output.begin(), output.end());

  fftw_destroy_plan(forward_plan);
  fftw_destroy_plan(backward_plan);

  return res;
}


signal_data<double>
calculate_inst_signal_data(std::vector<double> const &data, double sampling_rate)
{
  auto const num_samples = data.size();
  auto const analytic_signal = hilbert_transform(data);

  signal_data<double> res{num_samples};

  for (int i = 0; i < num_samples; ++i)
  {
    res.ampl[i] = std::abs(analytic_signal[i]);
    res.phase[i] = std::atan2(std::imag(analytic_signal[i]), std::real(analytic_signal[i]));
  }

  for (size_t i = 1; i < num_samples; ++i)
  {
    double constexpr tau = 2 * std::numbers::pi;

    double const delta_phase = res.phase[i] - res.phase[i - 1];
    double f = delta_phase * sampling_rate / tau;

    if (f < 0)
    {
      f += sampling_rate;
    }
    res.freq[i] = f;
  }

  res.freq[0] = res.freq[1];

  return res;
}

} // namespace hilbert
