#ifndef HILBERT_FREQ_SWEEP_ANALYSIS_HPP
#define HILBERT_FREQ_SWEEP_ANALYSIS_HPP


#include <hilbert/simulation/suspension/sinks/soa_vector.hpp>

#include <concepts>


namespace hilbert_freq_sweep
{

template<std::floating_point Float>
using frequency_sweep_simulation_data =
    typename hilbert::simulation::suspension::sinks::soa_vector_sink<Float>::simulation_data;


template<std::floating_point Float>
struct frequency_sweep_result
{
  Float frequency_hz;
  Float phase_fit_rad;
  Float phase_hilbert_rad;
};


// TODO: Replace this placeholder with the sinusoidal least-squares phase estimator.
template<std::floating_point Float>
Float
estimate_frequency_sweep_phase_by_least_squares(frequency_sweep_simulation_data<Float> const &samples)
{
  static_cast<void>(samples);
  return static_cast<Float>(10.0);
}


// TODO: Replace this placeholder with the Hilbert-transform phase estimator.
template<std::floating_point Float>
Float
estimate_frequency_sweep_phase_by_hilbert_transform(frequency_sweep_simulation_data<Float> const &samples)
{
  static_cast<void>(samples);
  return static_cast<Float>(10.0);
}

} // namespace hilbert_freq_sweep

#endif // HILBERT_FREQ_SWEEP_ANALYSIS_HPP
