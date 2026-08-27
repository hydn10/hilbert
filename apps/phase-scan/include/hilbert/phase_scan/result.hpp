#ifndef HILBERT_PHASE_SCAN_RESULT_HPP
#define HILBERT_PHASE_SCAN_RESULT_HPP


#include <hilbert/core/supported_float.hpp>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
struct result
{
  Float frequency_hz;
  Float magnitude_least_squares_n_per_m;
  Float magnitude_hilbert_n_per_m;
  Float phase_least_squares_rad;
  Float phase_hilbert_rad;
  Float least_squares_basis_condition_number;
  Float least_squares_ground_normalized_residual;
  Float least_squares_tire_force_normalized_residual;
  Float hilbert_mean_resultant_length;
  Float hilbert_magnitude_normalized_residual;
};

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_RESULT_HPP
