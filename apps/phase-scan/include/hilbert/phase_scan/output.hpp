#ifndef HILBERT_PHASE_SCAN_OUTPUT_HPP
#define HILBERT_PHASE_SCAN_OUTPUT_HPP


#include <hilbert/phase_scan/analysis.hpp>

#include <iomanip>
#include <limits>
#include <ostream>
#include <span>


namespace hilbert::phase_scan
{

template<std::floating_point Float>
void
write_phase_scan_results(std::ostream &output, std::span<phase_scan_result<Float> const> results);


template<std::floating_point Float>
void
write_phase_scan_results(std::ostream &output, std::span<phase_scan_result<Float> const> results)
{
  output << std::setprecision(std::numeric_limits<Float>::max_digits10);
  output << "# table: results\n"
            "frequency_hz,phase_fit_rad,phase_hilbert_rad\n";

  for (auto const &result : results)
  {
    output << result.frequency_hz << ',' << result.phase_fit_rad << ',' << result.phase_hilbert_rad << '\n';
  }
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_OUTPUT_HPP
