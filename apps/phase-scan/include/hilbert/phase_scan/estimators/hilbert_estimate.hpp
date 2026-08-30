#ifndef HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_ESTIMATE_HPP
#define HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_ESTIMATE_HPP


#include <hilbert/analysis/frequency_response.hpp>
#include <hilbert/analysis/phase/circular_mean.hpp>
#include <hilbert/analysis/phase/circular_mean_result.hpp>
#include <hilbert/analysis/sampling/sample_window.hpp>
#include <hilbert/analysis/signals/hilbert_transform.hpp>
#include <hilbert/analysis/signals/remove_dc_component.hpp>
#include <hilbert/core/supported_float.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
struct hilbert_diagnostics
{
  hilbert::analysis::mean_resultant_length<Float> mean_resultant_length;
  Float magnitude_normalized_residual;
};


namespace detail
{

template<hilbert::supported_float Float>
struct analytic_response_summary
{
  hilbert::analysis::frequency_response<Float> response;
  hilbert::analysis::mean_resultant_length<Float> mean_resultant_length;
};


template<hilbert::supported_float Float>
[[nodiscard]]
analytic_response_summary<Float>
summarize_analytic_response(
    std::span<std::complex<Float> const> ground, std::span<std::complex<Float> const> tire_force);


template<hilbert::supported_float Float>
[[nodiscard]]
Float
magnitude_normalized_residual(
    std::span<std::complex<Float> const> ground,
    std::span<std::complex<Float> const> tire_force,
    Float magnitude_ratio);

} // namespace detail


template<hilbert::supported_float Float>
class hilbert_estimate
{
  hilbert::analysis::frequency_response<Float> response_;
  hilbert_diagnostics<Float> diagnostics_;

public:
  hilbert_estimate(hilbert::analysis::frequency_response<Float> response, hilbert_diagnostics<Float> diagnostics);

  [[nodiscard]]
  hilbert::analysis::frequency_response<Float> const &
  response() const noexcept;

  [[nodiscard]]
  hilbert_diagnostics<Float> const &
  diagnostics() const noexcept;
};


template<hilbert::supported_float Float>
[[nodiscard]]
hilbert_estimate<Float>
estimate_phase_scan_by_hilbert_transform(
    std::span<Float const> ground,
    std::span<Float const> tire_force,
    hilbert::analysis::sample_range const &measurement);


template<hilbert::supported_float Float>
detail::analytic_response_summary<Float>
detail::summarize_analytic_response(
    std::span<std::complex<Float> const> ground, std::span<std::complex<Float> const> tire_force)
{
  Float ground_amplitude_sum{};
  Float tire_force_amplitude_sum{};
  hilbert::analysis::circular_mean<Float> relative_phase;

  for (auto const &[ground_value, tire_force_value] : std::views::zip(ground, tire_force))
  {
    auto const ground_amplitude = std::abs(ground_value);
    auto const tire_force_amplitude = std::abs(tire_force_value);

    ground_amplitude_sum += ground_amplitude;
    tire_force_amplitude_sum += tire_force_amplitude;
    relative_phase.add_vector(tire_force_value * std::conj(ground_value));
  }

  if (ground_amplitude_sum == 0)
  {
    throw std::invalid_argument{"Hilbert magnitude ratio has zero ground amplitude"};
  }

  auto const phase = relative_phase.finish();
  auto const magnitude_ratio = tire_force_amplitude_sum / ground_amplitude_sum;
  auto const response =
      hilbert::analysis::frequency_response<Float>{std::polar(magnitude_ratio, phase.phase().radians())};

  return {response, phase.resultant_length()};
}


template<hilbert::supported_float Float>
Float
detail::magnitude_normalized_residual(
    std::span<std::complex<Float> const> ground, std::span<std::complex<Float> const> tire_force, Float magnitude_ratio)
{
  Float residual_squared_norm{};
  Float tire_force_squared_norm{};

  for (auto const &[ground_value, tire_force_value] : std::views::zip(ground, tire_force))
  {
    auto const ground_amplitude = std::abs(ground_value);
    auto const tire_force_amplitude = std::abs(tire_force_value);
    auto const residual = std::fma(-magnitude_ratio, ground_amplitude, tire_force_amplitude);
    residual_squared_norm = std::fma(residual, residual, residual_squared_norm);
    tire_force_squared_norm = std::fma(tire_force_amplitude, tire_force_amplitude, tire_force_squared_norm);
  }

  if (tire_force_squared_norm == 0)
  {
    return residual_squared_norm == 0 ? static_cast<Float>(0) : static_cast<Float>(1);
  }

  // The envelope residual avoids division by small instantaneous ground amplitudes.
  return static_cast<Float>(std::sqrt(residual_squared_norm / tire_force_squared_norm));
}


template<hilbert::supported_float Float>
hilbert_estimate<Float>::hilbert_estimate(
    hilbert::analysis::frequency_response<Float> response, hilbert_diagnostics<Float> diagnostics)
    : response_{response}
    , diagnostics_{diagnostics}
{
}


template<hilbert::supported_float Float>
hilbert::analysis::frequency_response<Float> const &
hilbert_estimate<Float>::response() const noexcept
{
  return response_;
}


template<hilbert::supported_float Float>
hilbert_diagnostics<Float> const &
hilbert_estimate<Float>::diagnostics() const noexcept
{
  return diagnostics_;
}


template<hilbert::supported_float Float>
hilbert_estimate<Float>
estimate_phase_scan_by_hilbert_transform(
    std::span<Float const> ground,
    std::span<Float const> tire_force,
    hilbert::analysis::sample_range const &measurement)
{
  auto const ground_dc = hilbert::analysis::estimate_dc_component(measurement.slice(ground));
  auto const tire_force_dc = hilbert::analysis::estimate_dc_component(measurement.slice(tire_force));

  auto const centered_ground = ground | std::views::transform(hilbert::analysis::remove_dc_component(ground_dc)) |
                               std::ranges::to<std::vector>();
  auto const centered_tire_force = tire_force |
                                   std::views::transform(hilbert::analysis::remove_dc_component(tire_force_dc)) |
                                   std::ranges::to<std::vector>();

  auto const analytic_ground = ::hilbert::hilbert_transform<Float>(centered_ground);
  auto const analytic_tire_force = ::hilbert::hilbert_transform<Float>(centered_tire_force);

  auto const ground_window = measurement.slice(std::span<std::complex<Float> const>{analytic_ground});
  auto const tire_force_window = measurement.slice(std::span<std::complex<Float> const>{analytic_tire_force});

  auto const summary = detail::summarize_analytic_response<Float>(ground_window, tire_force_window);
  auto const magnitude_normalized_residual =
      detail::magnitude_normalized_residual<Float>(ground_window, tire_force_window, summary.response.magnitude());
  auto const diagnostics = hilbert_diagnostics<Float>{
      .mean_resultant_length = summary.mean_resultant_length,
      .magnitude_normalized_residual = magnitude_normalized_residual,
  };

  return {summary.response, diagnostics};
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_ESTIMATE_HPP
