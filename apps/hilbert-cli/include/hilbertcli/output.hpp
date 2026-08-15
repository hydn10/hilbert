#ifndef HILBERTCLI_OUTPUT_HPP
#define HILBERTCLI_OUTPUT_HPP


#include <hilbertcli/simulation.hpp>

#include <hilbert/hilbert.hpp>

#include <iomanip>
#include <limits>
#include <ostream>
#include <ranges>


namespace hilbertcli
{

template<hilbert::supported_float Float>
void
write_simulation_data(
    std::ostream &output, simulation_data<Float> const &samples, analysis_result<Float> const &analysis)
{
  output << std::setprecision(std::numeric_limits<Float>::max_digits10);

  output << "# table: intervals\n"
            "measurement_start_s,measurement_end_s,hilbert_start_s,hilbert_end_s\n"
         << analysis.measurement_interval.start_time << ',' << analysis.measurement_interval.end_time << ','
         << analysis.hilbert_interval.start_time << ',' << analysis.hilbert_interval.end_time << "\n\n";

  output << "# table: raw\n"
            "time_s,sprung_displacement_m,unsprung_displacement_m,platform_displacement_m,tire_force_n\n";

  auto const time = samples.time_span();
  auto const sprung = samples.sprung_displacement_span();
  auto const unsprung = samples.unsprung_displacement_span();
  auto const platform = samples.ground_displacement_span();
  auto const tire_force = samples.tire_force_span();
  for (
      auto const &[time_value, sprung_value, unsprung_value, platform_value, tire_force_value] :
      std::views::zip(time, sprung, unsprung, platform, tire_force))
  {
    output << time_value << ',' << sprung_value << ',' << unsprung_value << ',' << platform_value << ','
           << tire_force_value << '\n';
  }

  output << "\n# table: refined\n"
            "time_s,platform_amplitude_m,platform_phase_rad,platform_frequency_hz,tire_force_amplitude_n,"
            "tire_force_phase_rad,tire_force_frequency_hz\n";

  auto const refined_time = samples.time_span().subspan(analysis.hilbert_offset, analysis.hilbert_size);
  for (
      auto const
          &[time_value,
            platform_amplitude,
            platform_phase,
            platform_frequency,
            tire_force_amplitude,
            tire_force_phase,
            tire_force_frequency] :
      std::views::zip(
          refined_time,
          analysis.platform_signal.ampl,
          analysis.platform_signal.phase,
          analysis.platform_signal.freq,
          analysis.tire_force_signal.ampl,
          analysis.tire_force_signal.phase,
          analysis.tire_force_signal.freq))
  {
    output << time_value << ',' << platform_amplitude << ',' << platform_phase << ',' << platform_frequency << ','
           << tire_force_amplitude << ',' << tire_force_phase << ',' << tire_force_frequency << '\n';
  }
}

} // namespace hilbertcli

#endif // HILBERTCLI_OUTPUT_HPP
