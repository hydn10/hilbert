#ifndef HILBERT_PHASE_SCAN_RECORD_HPP
#define HILBERT_PHASE_SCAN_RECORD_HPP


#include <hilbert/analysis/sampling/sample_window.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/simulation/suspension/sinks/soa_vector.hpp>

#include <span>
#include <stdexcept>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
using phase_scan_simulation_data = hilbert::simulation::suspension::sinks::soa_vector_sink<Float>::simulation_data;


template<hilbert::supported_float Float>
class phase_scan_record_view
{
  std::span<Float const> time_;
  std::span<Float const> ground_;
  std::span<Float const> force_;

  hilbert::analysis::sample_range measurement_;

  static std::span<Float const>
  validate_and_get_time(phase_scan_simulation_data<Float> const &samples)
  {
    auto const time = samples.time_span();
    if (time.size() != samples.ground_displacement_span().size() || time.size() != samples.tire_force_span().size())
    {
      throw std::invalid_argument{"phase-scan channels must have equal lengths"};
    }
    return time;
  }

public:
  phase_scan_record_view(
      phase_scan_simulation_data<Float> const &samples, hilbert::analysis::time_window<Float> measurement_window)
      : time_{validate_and_get_time(samples)}
      , ground_{samples.ground_displacement_span()}
      , force_{samples.tire_force_span()}
      , measurement_{hilbert::analysis::select_sample_range(time_, measurement_window)}
  {
  }

  [[nodiscard]]
  std::span<Float const>
  time() const noexcept
  {
    return time_;
  }

  [[nodiscard]]
  std::span<Float const>
  ground() const noexcept
  {
    return ground_;
  }

  [[nodiscard]]
  std::span<Float const>
  force() const noexcept
  {
    return force_;
  }

  [[nodiscard]]
  hilbert::analysis::sample_range const &
  measurement_range() const noexcept
  {
    return measurement_;
  }

  [[nodiscard]]
  std::span<Float const>
  measurement_time() const
  {
    return measurement_.slice(time_);
  }

  [[nodiscard]]
  std::span<Float const>
  measurement_ground() const
  {
    return measurement_.slice(ground_);
  }

  [[nodiscard]]
  std::span<Float const>
  measurement_force() const
  {
    return measurement_.slice(force_);
  }
};

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_RECORD_HPP
