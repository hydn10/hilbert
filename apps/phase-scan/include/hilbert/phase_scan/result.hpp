#ifndef HILBERT_PHASE_SCAN_RESULT_HPP
#define HILBERT_PHASE_SCAN_RESULT_HPP


#include <hilbert/core/supported_float.hpp>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
struct result
{
  Float frequency_hz;
  Float magnitude_fit_n_per_m;
  Float magnitude_hilbert_n_per_m;
  Float phase_fit_rad;
  Float phase_hilbert_rad;
};

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_RESULT_HPP
