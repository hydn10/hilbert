#ifndef HILBERT_PHASE_SCAN_ANALYSIS_HPP
#define HILBERT_PHASE_SCAN_ANALYSIS_HPP


#include <hilbert/analysis/phase.hpp>
#include <hilbert/simulation/suspension/sinks/soa_vector.hpp>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
using phase_scan_simulation_data = hilbert::simulation::suspension::sinks::soa_vector_sink<Float>::simulation_data;


template<hilbert::supported_float Float>
struct phase_scan_result
{
  Float frequency_hz;
  Float phase_fit_rad;
  Float phase_hilbert_rad;
};


template<hilbert::supported_float Float>
Float
estimate_phase_scan_by_least_squares(
    phase_scan_simulation_data<Float> const &samples, Float frequency_hz, hilbert::analysis::time_window<Float> window);


template<hilbert::supported_float Float>
Float
estimate_phase_scan_by_hilbert_transform(
    phase_scan_simulation_data<Float> const &samples, hilbert::analysis::time_window<Float> window);


template<hilbert::supported_float Float>
Float
estimate_phase_scan_by_least_squares(
    phase_scan_simulation_data<Float> const &samples, Float frequency_hz, hilbert::analysis::time_window<Float> window)
{
  return hilbert::analysis::fit_sinusoidal_phase(
             samples.time_span(), samples.ground_displacement_span(), samples.tire_force_span(), frequency_hz, window)
      .phase_rad;
}


template<hilbert::supported_float Float>
Float
estimate_phase_scan_by_hilbert_transform(
    phase_scan_simulation_data<Float> const &samples, hilbert::analysis::time_window<Float> window)
{
  return hilbert::analysis::estimate_hilbert_phase(
             samples.time_span(), samples.ground_displacement_span(), samples.tire_force_span(), window)
      .phase_rad;
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ANALYSIS_HPP
