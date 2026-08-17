#ifndef HILBERT_PHASE_SCAN_ANALYSIS_HPP
#define HILBERT_PHASE_SCAN_ANALYSIS_HPP


#include <hilbert/simulation/suspension/sinks/soa_vector.hpp>

#include <concepts>


namespace hilbert::phase_scan
{

template<std::floating_point Float>
using phase_scan_simulation_data = hilbert::simulation::suspension::sinks::soa_vector_sink<Float>::simulation_data;


template<std::floating_point Float>
struct phase_scan_result
{
  Float frequency_hz;
  Float phase_fit_rad;
  Float phase_hilbert_rad;
};


template<std::floating_point Float>
Float
estimate_phase_scan_by_least_squares(phase_scan_simulation_data<Float> const &samples);


template<std::floating_point Float>
Float
estimate_phase_scan_by_hilbert_transform(phase_scan_simulation_data<Float> const &samples);


// TODO: Replace this placeholder with the sinusoidal least-squares phase estimator.
template<std::floating_point Float>
Float
estimate_phase_scan_by_least_squares(phase_scan_simulation_data<Float> const &samples)
{
  static_cast<void>(samples);
  return static_cast<Float>(10.0);
}


// TODO: Replace this placeholder with the Hilbert-transform phase estimator.
template<std::floating_point Float>
Float
estimate_phase_scan_by_hilbert_transform(phase_scan_simulation_data<Float> const &samples)
{
  static_cast<void>(samples);
  return static_cast<Float>(10.0);
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_ANALYSIS_HPP
