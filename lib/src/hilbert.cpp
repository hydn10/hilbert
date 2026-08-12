#include <hilbert/hilbert.hpp>

#include <hilbert/detail/fft.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <limits>
#include <numbers>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>


namespace hilbert
{

namespace
{

auto
take_positive_freqs(size_t n)
{
  return std::views::drop(1) | std::views::take((n - 1uz) / 2uz);
}


auto
take_negative_freqs(size_t n)
{
  return std::views::reverse | std::views::take((n - 1uz) / 2uz);
}


inline constexpr auto drop_transform = [](auto &x)
{
  x = 0;
};


inline constexpr auto hilbert_positive_freqs_transform = [](auto &x)
{
  x *= 2;
};


inline constexpr auto hilbert_negative_freqs_transform = drop_transform;


size_t
cutoff_bin(double frequency, size_t num_samples, double sampling_rate)
{
  return static_cast<size_t>(frequency * static_cast<double>(num_samples) / sampling_rate);
}


auto
take_positive_freqs_greater_than(double frequency, size_t num_samples, double sampling_rate)
{
  auto const cutoff = cutoff_bin(frequency, num_samples, sampling_rate);

  auto const num_to_drop = cutoff + 1uz;
  auto const num_to_take = ((num_samples - 1uz) / 2uz) - cutoff;

  return std::views::drop(num_to_drop) | std::views::take(num_to_take);
}


auto
take_negative_freqs_greater_than(double frequency, size_t num_samples, double sampling_rate)
{
  auto const cutoff = cutoff_bin(frequency, num_samples, sampling_rate);

  auto const num_to_drop = cutoff;
  auto const num_to_take = ((num_samples - 1uz) / 2uz) - cutoff;

  return std::views::reverse | std::views::drop(num_to_drop) | std::views::take(num_to_take);
}


double
principal_phase_delta(double previous_phase, double current_phase)
{
  double constexpr tau = 2 * std::numbers::pi;
  return std::remainder(current_phase - previous_phase, tau);
}

} // namespace


std::vector<std::complex<double>>
hilbert_transform(std::span<double const> input)
{
  auto const n = input.size();

  if (n < 2uz)
  {
    throw std::invalid_argument{"hilbert_transform requires at least two samples"};
  }

  if (n > static_cast<size_t>(std::numeric_limits<int>::max()))
  {
    throw std::length_error{"hilbert_transform input exceeds FFTW's one-dimensional size limit"};
  }

  auto freq_data = detail::fft::transform(input);

  std::ranges::for_each(freq_data | take_positive_freqs(n), hilbert_positive_freqs_transform);
  std::ranges::for_each(freq_data | take_negative_freqs(n), hilbert_negative_freqs_transform);

  auto time_data = detail::fft::transform(freq_data, detail::fft::sign::backward);

  std::ranges::for_each(
      time_data,
      [n](auto &x)
      {
        x /= static_cast<double>(n);
      });

  return time_data;
}


signal_data<double>
calculate_inst_signal_data(std::span<double const> data, double sampling_rate)
{
  if (!std::isfinite(sampling_rate) || sampling_rate <= 0)
  {
    throw std::invalid_argument{"sampling_rate must be finite and positive"};
  }

  auto const num_samples = data.size();
  auto const analytic_signal = hilbert_transform(data);

  signal_data<double> res{num_samples};

  for (auto [sample, amplitude, phase] : std::views::zip(analytic_signal, res.ampl, res.phase))
  {
    amplitude = std::abs(sample);
    phase = std::arg(sample);
  }

  auto const phase_pairs = res.phase | std::views::adjacent<2>;
  auto const frequencies = res.freq | std::views::drop(1);

  for (auto [phases, frequency] : std::views::zip(phase_pairs, frequencies))
  {
    double constexpr tau = 2 * std::numbers::pi;

    auto const &[previous_phase, current_phase] = phases;
    double const delta_phase = principal_phase_delta(previous_phase, current_phase);
    frequency = delta_phase * sampling_rate / tau;
  }

  auto first_frequency = res.freq.begin();
  auto second_frequency = first_frequency;
  ++second_frequency;
  *first_frequency = *second_frequency;

  return res;
}

} // namespace hilbert
