#ifndef HILBERT_FREQ_SWEEP_OUTPUT_HPP
#define HILBERT_FREQ_SWEEP_OUTPUT_HPP


#include <hilbert_freq_sweep/analysis.hpp>

#include <iomanip>
#include <limits>
#include <ostream>
#include <span>


namespace hilbert_freq_sweep
{

template<std::floating_point Float>
void
write_frequency_sweep_results(std::ostream &output, std::span<frequency_sweep_result<Float> const> results)
{
  output << std::setprecision(std::numeric_limits<Float>::max_digits10);
  output << "# table: results\n"
            "frequency_hz,phase_fit_rad,phase_hilbert_rad\n";

  for (auto const &result : results)
  {
    output << result.frequency_hz << ',' << result.phase_fit_rad << ',' << result.phase_hilbert_rad << '\n';
  }
}

} // namespace hilbert_freq_sweep

#endif // HILBERT_FREQ_SWEEP_OUTPUT_HPP
