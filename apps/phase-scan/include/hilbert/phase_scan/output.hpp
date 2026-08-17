#ifndef HILBERT_PHASE_SCAN_OUTPUT_HPP
#define HILBERT_PHASE_SCAN_OUTPUT_HPP


#include <hilbert/app/io/numeric_table_writer.hpp>
#include <hilbert/phase_scan/analysis.hpp>

#include <array>
#include <ostream>
#include <span>
#include <string_view>


namespace hilbert::phase_scan
{

template<std::floating_point Float>
void
write_phase_scan_results(std::ostream &output, std::span<phase_scan_result<Float> const> results);


template<std::floating_point Float>
void
write_phase_scan_results(std::ostream &output, std::span<phase_scan_result<Float> const> results)
{
  hilbert::app::io::numeric_table_writer<Float> document{output};

  constexpr std::array result_columns{
      std::string_view{"frequency_hz"},
      std::string_view{"phase_fit_rad"},
      std::string_view{"phase_hilbert_rad"},
  };

  document.table(
      "results",
      result_columns,
      [&](auto &rows)
      {
        for (auto const &result : results)
        {
          rows.write(result.frequency_hz, result.phase_fit_rad, result.phase_hilbert_rad);
        }
      });
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_OUTPUT_HPP
