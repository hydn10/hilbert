#ifndef HILBERT_PHASE_SCAN_RESULT_HPP
#define HILBERT_PHASE_SCAN_RESULT_HPP


#include <hilbert/core/supported_float.hpp>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
struct phase_scan_result
{
  Float frequency_hz;
  Float phase_fit_rad;
  Float phase_hilbert_rad;
};

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_RESULT_HPP
