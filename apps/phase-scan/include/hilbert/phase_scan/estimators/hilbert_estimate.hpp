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


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
struct hilbert_diagnostics
{
  hilbert::analysis::mean_resultant_length<Float> mean_resultant_length;
  Float gain_coefficient_of_variation;
};


namespace detail
{

template<hilbert::supported_float Float>
struct analytic_response_summary
{
  hilbert::analysis::frequency_response<Float> response;
  hilbert::analysis::mean_resultant_length<Float> mean_resultant_length;
  Float maximum_ground_amplitude;
};


template<hilbert::supported_float Float>
[[nodiscard]]
analytic_response_summary<Float>
summarize_analytic_response(
    std::span<std::complex<Float> const> ground, std::span<std::complex<Float> const> tire_force);


template<hilbert::supported_float Float>
[[nodiscard]]
Float
instantaneous_gain_coefficient_of_variation(
    std::span<std::complex<Float> const> ground,
    std::span<std::complex<Float> const> tire_force,
    Float maximum_ground_amplitude);

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
  Float maximum_ground_amplitude{};
  hilbert::analysis::circular_mean<Float> relative_phase;

  for (auto const &[ground_value, tire_force_value] : std::views::zip(ground, tire_force))
  {
    auto const ground_amplitude = std::abs(ground_value);
    auto const tire_force_amplitude = std::abs(tire_force_value);

    ground_amplitude_sum += ground_amplitude;
    tire_force_amplitude_sum += tire_force_amplitude;
    maximum_ground_amplitude = std::max(maximum_ground_amplitude, ground_amplitude);
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

  return {response, phase.resultant_length(), maximum_ground_amplitude};
}


template<hilbert::supported_float Float>
Float
detail::instantaneous_gain_coefficient_of_variation(
    std::span<std::complex<Float> const> ground,
    std::span<std::complex<Float> const> tire_force,
    Float maximum_ground_amplitude)
{
  constexpr auto relative_ground_amplitude_floor = static_cast<Float>(1e-12);
  auto const ground_amplitude_floor = maximum_ground_amplitude * relative_ground_amplitude_floor;

  Float gain_mean{};
  Float gain_m2{};
  std::size_t gain_count{};

  for (auto const &[ground_value, tire_force_value] : std::views::zip(ground, tire_force))
  {
    auto const ground_amplitude = std::abs(ground_value);
    if (ground_amplitude <= ground_amplitude_floor)
    {
      continue;
    }

    auto const gain = std::abs(tire_force_value) / ground_amplitude;
    ++gain_count;
    auto const delta = gain - gain_mean;
    gain_mean += delta / static_cast<Float>(gain_count);
    gain_m2 += delta * (gain - gain_mean);
  }

  if (gain_count == 0uz)
  {
    throw std::invalid_argument{"Hilbert gain variation has no valid ground amplitude samples"};
  }

  auto const gain_variance = gain_m2 / static_cast<Float>(gain_count);
  if (gain_mean == 0)
  {
    return static_cast<Float>(0);
  }

  return std::sqrt(std::max(static_cast<Float>(0), gain_variance)) / gain_mean;
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
  auto const centered_ground = hilbert::analysis::remove_dc_component(ground);
  auto const centered_tire_force = hilbert::analysis::remove_dc_component(tire_force);
  auto const analytic_ground = ::hilbert::hilbert_transform<Float>(std::span<Float const>{centered_ground});
  auto const analytic_tire_force = ::hilbert::hilbert_transform<Float>(std::span<Float const>{centered_tire_force});

  auto const ground_window = measurement.slice(std::span<std::complex<Float> const>{analytic_ground});
  auto const tire_force_window = measurement.slice(std::span<std::complex<Float> const>{analytic_tire_force});
  auto const summary = detail::summarize_analytic_response<Float>(ground_window, tire_force_window);
  auto const gain_coefficient_of_variation = detail::instantaneous_gain_coefficient_of_variation<Float>(
      ground_window, tire_force_window, summary.maximum_ground_amplitude);
  auto const diagnostics = hilbert_diagnostics<Float>{
      .mean_resultant_length = summary.mean_resultant_length,
      .gain_coefficient_of_variation = gain_coefficient_of_variation,
  };

  return {summary.response, diagnostics};
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ESTIMATORS_HILBERT_ESTIMATE_HPP
