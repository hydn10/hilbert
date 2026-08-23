#include <hilbert/analysis/signals/instantaneous_signal.hpp>

#include <hilbert/analysis/signals/hilbert_transform.hpp>
#include <hilbert/core/supported_float.hpp>

#include <cmath>
#include <numbers>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>


namespace hilbert
{
namespace
{

template<supported_float Float>
Float
principal_phase_delta(Float previous_phase, Float current_phase)
{
  Float constexpr tau = 2 * std::numbers::pi_v<Float>;
  return std::remainder(current_phase - previous_phase, tau);
}

} // namespace


template<supported_float Float>
signal_data<Float>
calculate_inst_signal_data(std::span<Float const> data, Float sampling_rate)
{
  if (!std::isfinite(sampling_rate) || sampling_rate <= 0)
  {
    throw std::invalid_argument{"sampling_rate must be finite and positive"};
  }

  auto const sample_count = data.size();
  auto const analytic_signal = hilbert_transform(data);

  std::vector<Float> amplitudes(sample_count);
  std::vector<Float> phases(sample_count);
  std::vector<Float> frequencies(sample_count);

  for (auto [sample, amplitude, phase] : std::views::zip(analytic_signal, amplitudes, phases))
  {
    amplitude = std::abs(sample);
    phase = std::arg(sample);
  }

  auto const phase_pairs = phases | std::views::adjacent<2>;
  auto const frequency_tail = frequencies | std::views::drop(1);

  for (auto [phase_pair, frequency] : std::views::zip(phase_pairs, frequency_tail))
  {
    Float constexpr tau = 2 * std::numbers::pi_v<Float>;

    auto const &[previous_phase, current_phase] = phase_pair;
    auto const delta_phase = principal_phase_delta(previous_phase, current_phase);
    frequency = delta_phase * sampling_rate / tau;
  }

  auto first_frequency = frequencies.begin();
  auto second_frequency = first_frequency;
  ++second_frequency;
  *first_frequency = *second_frequency;

  return signal_data<Float>{std::move(amplitudes), std::move(phases), std::move(frequencies)};
}


template signal_data<double>
calculate_inst_signal_data<double>(std::span<double const> data, double sampling_rate);

} // namespace hilbert
