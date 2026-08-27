#ifndef HILBERT_PHASE_SCAN_OUTPUT_HPP
#define HILBERT_PHASE_SCAN_OUTPUT_HPP


#include <hilbert/app/io/numeric_table_writer.hpp>
#include <hilbert/phase_scan/result.hpp>

#include <array>
#include <ostream>
#include <span>
#include <string_view>


namespace hilbert::phase_scan
{

template<hilbert::supported_float Float>
void
write_results(std::ostream &output, std::span<result<Float> const> results);


template<hilbert::supported_float Float>
void
write_results(std::ostream &output, std::span<result<Float> const> results)
{
  hilbert::app::io::numeric_table_writer<Float> document{output};

  constexpr std::array result_columns{
      std::string_view{"frequency_hz"},
      std::string_view{"magnitude_least_squares_n_per_m"},
      std::string_view{"magnitude_hilbert_n_per_m"},
      std::string_view{"phase_least_squares_rad"},
      std::string_view{"phase_hilbert_rad"},
      std::string_view{"least_squares_basis_condition_number"},
      std::string_view{"least_squares_ground_normalized_residual"},
      std::string_view{"least_squares_tire_force_normalized_residual"},
      std::string_view{"hilbert_mean_resultant_length"},
      std::string_view{"hilbert_magnitude_normalized_residual"},
  };

  document.table(
      "results",
      result_columns,
      [&](auto &rows)
      {
        for (auto const &result : results)
        {
          rows.write(
              result.frequency_hz,
              result.magnitude_least_squares_n_per_m,
              result.magnitude_hilbert_n_per_m,
              result.phase_least_squares_rad,
              result.phase_hilbert_rad,
              result.least_squares_basis_condition_number,
              result.least_squares_ground_normalized_residual,
              result.least_squares_tire_force_normalized_residual,
              result.hilbert_mean_resultant_length,
              result.hilbert_magnitude_normalized_residual);
        }
      });
}

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_OUTPUT_HPP
