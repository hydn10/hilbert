#ifndef HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_ESTIMATE_HPP
#define HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_ESTIMATE_HPP


#include <hilbert/analysis/phase/circular_mean.hpp>
#include <hilbert/analysis/phase/mean_resultant_length.hpp>
#include <hilbert/analysis/frequency_response.hpp>
#include <hilbert/analysis/sampling/sample_window.hpp>
#include <hilbert/analysis/signals/hilbert_transform.hpp>
#include <hilbert/analysis/signals/remove_dc_component.hpp>
#include <hilbert/core/supported_float.hpp>

#include <complex>
#include <ranges>
#include <span>
#include <stdexcept>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
class hilbert_estimate
{
  hilbert::analysis::frequency_response<Float> response_;
  hilbert::analysis::mean_resultant_length<Float> resultant_length_;

public:
  hilbert_estimate(
      hilbert::analysis::frequency_response<Float> response,
      hilbert::analysis::mean_resultant_length<Float> resultant_length);

  [[nodiscard]]
  hilbert::analysis::frequency_response<Float> const &
  response() const noexcept;

  [[nodiscard]]
  hilbert::analysis::mean_resultant_length<Float>
  resultant_length() const noexcept;
};


template<hilbert::supported_float Float>
hilbert_estimate<Float>::hilbert_estimate(
    hilbert::analysis::frequency_response<Float> response,
    hilbert::analysis::mean_resultant_length<Float> resultant_length)
    : response_{response}
    , resultant_length_{resultant_length}
{
}


template<hilbert::supported_float Float>
hilbert::analysis::frequency_response<Float> const &
hilbert_estimate<Float>::response() const noexcept
{
  return response_;
}


template<hilbert::supported_float Float>
hilbert::analysis::mean_resultant_length<Float>
hilbert_estimate<Float>::resultant_length() const noexcept
{
  return resultant_length_;
}


template<hilbert::supported_float Float>
hilbert_estimate<Float>
estimate_phase_scan_by_hilbert_transform(
    std::span<Float const> ground, std::span<Float const> force, hilbert::analysis::sample_range const &measurement);


template<hilbert::supported_float Float>
hilbert_estimate<Float>
estimate_phase_scan_by_hilbert_transform(
    std::span<Float const> ground, std::span<Float const> force, hilbert::analysis::sample_range const &measurement)
{
  auto const centered_ground = hilbert::analysis::remove_dc_component(ground);
  auto const centered_force = hilbert::analysis::remove_dc_component(force);
  auto const analytic_ground = ::hilbert::hilbert_transform<Float>(std::span<Float const>{centered_ground});
  auto const analytic_force = ::hilbert::hilbert_transform<Float>(std::span<Float const>{centered_force});

  auto const ground_window = measurement.slice(std::span<std::complex<Float> const>{analytic_ground});
  auto const force_window = measurement.slice(std::span<std::complex<Float> const>{analytic_force});

  Float ground_amplitude_sum{};
  Float force_amplitude_sum{};
  hilbert::analysis::circular_mean<Float> mean;

  for (auto const &[ground_value, force_value] : std::views::zip(ground_window, force_window))
  {
    auto const ground_amplitude = std::abs(ground_value);
    auto const force_amplitude = std::abs(force_value);

    auto const relative_vector = force_value * std::conj(ground_value);

    ground_amplitude_sum += ground_amplitude;
    force_amplitude_sum += force_amplitude;

    mean.add_vector(relative_vector);
  }

  if (ground_amplitude_sum == 0)
  {
    throw std::invalid_argument{"Hilbert magnitude ratio has zero ground amplitude"};
  }

  auto const magnitude_ratio = force_amplitude_sum / ground_amplitude_sum;
  auto const circular_mean = mean.finish();
  
  auto const response = hilbert::analysis::frequency_response<Float>{
      std::polar(magnitude_ratio, circular_mean.phase().radians())};

  return {response, circular_mean.resultant_length()};
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_ESTIMATE_HPP
