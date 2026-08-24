#ifndef HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_HPP
#define HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_HPP


#include <hilbert/analysis/phase/circular_mean.hpp>
#include <hilbert/analysis/phase/relative_phase_estimate.hpp>
#include <hilbert/analysis/sampling/sample_window.hpp>
#include <hilbert/analysis/signals/hilbert_transform.hpp>
#include <hilbert/analysis/signals/remove_dc_component.hpp>
#include <hilbert/core/supported_float.hpp>

#include <complex>
#include <ranges>
#include <span>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
hilbert::analysis::relative_phase_estimate<Float>
estimate_phase_scan_by_hilbert_transform(
    std::span<Float const> ground, std::span<Float const> force, hilbert::analysis::sample_range const &measurement);


template<hilbert::supported_float Float>
hilbert::analysis::relative_phase_estimate<Float>
estimate_phase_scan_by_hilbert_transform(
    std::span<Float const> ground, std::span<Float const> force, hilbert::analysis::sample_range const &measurement)
{
  auto const centered_ground = hilbert::analysis::remove_dc_component(ground);
  auto const centered_force = hilbert::analysis::remove_dc_component(force);
  auto const analytic_ground = ::hilbert::hilbert_transform<Float>(std::span<Float const>{centered_ground});
  auto const analytic_force = ::hilbert::hilbert_transform<Float>(std::span<Float const>{centered_force});

  auto const ground_window = measurement.slice(std::span<std::complex<Float> const>{analytic_ground});
  auto const force_window = measurement.slice(std::span<std::complex<Float> const>{analytic_force});

  hilbert::analysis::circular_mean<Float> mean;
  for (auto const &[ground_value, force_value] : std::views::zip(ground_window, force_window))
  {
    mean.add_relative_vector(ground_value * std::conj(force_value));
  }
  return mean.finish();
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_HPP
