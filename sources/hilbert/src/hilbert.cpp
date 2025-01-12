#include <hilbert/hilbert.hpp>

#include <hilbert/fft.hpp>

#include <fftw3.h>

#include <algorithm>
#include <complex>
#include <memory>
#include <numbers>
#include <ranges>
#include <vector>


namespace hilbert
{

namespace
{

auto
take_positive_freqs(size_t n)
{
  return std::views::drop(1) | std::views::take((n - 1) / 2);
}


auto
take_negative_freqs(size_t n)
{
  return std::views::reverse | std::views::take((n - 1) / 2);
}


inline constexpr auto drop_transform = [](auto &x)
{
  x[0] = 0;
  x[1] = 0;
};


inline constexpr auto hilbert_positive_freqs_transform = [](auto &x)
{
  x[0] *= 2;
  x[1] *= 2;
};


inline constexpr auto hilbert_negative_freqs_transform = drop_transform;


size_t
cutoff_bin(double frequency, size_t num_samples, double sampling_rate)
{
  return static_cast<size_t>(frequency * num_samples / sampling_rate);
}


auto
take_positive_freqs_greater_than(double frequency, size_t num_samples, double sampling_rate)
{
  auto const cutoff = cutoff_bin(frequency, num_samples, sampling_rate);

  auto const num_to_drop = cutoff + 1;
  auto const num_to_take = ((num_samples - 1) / 2) - cutoff;

  return std::views::drop(num_to_drop) | std::views::take(num_to_take);
}


auto
take_negative_freqs_greater_than(double frequency, size_t num_samples, double sampling_rate)
{
  auto const cutoff = cutoff_bin(frequency, num_samples, sampling_rate);

  auto const num_to_drop = cutoff;
  auto const num_to_take = ((num_samples - 1) / 2) - cutoff;

  return std::views::reverse | std::views::drop(num_to_drop) | std::views::take(num_to_take);
}

} // namespace


std::vector<std::complex<double>>
hilbert_transform(std::vector<double> const &input)
{
  auto const n = input.size();

  auto freq_data = fft::fft_transform(input);

  std::ranges::for_each(freq_data | take_positive_freqs(n), hilbert_positive_freqs_transform);
  std::ranges::for_each(freq_data | take_negative_freqs(n), hilbert_negative_freqs_transform);

  auto const time_data = fft::fft_transform(freq_data, fft::sign::BACKWARD);

  return time_data | std::views::transform([n](auto &x) { return std::complex<double>(x[0] / n, x[1] / n); }) |
         std::ranges::to<std::vector>();
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
    res.phase[i] = std::arg(analytic_signal[i]);
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
