#include <hilbertcli/output.hpp>

#include <hilbertcli/simulation.hpp>

#include <iomanip>
#include <ostream>
#include <ranges>


namespace hilbertcli
{

void
write_simulation_data(std::ostream &output, simulation_result const &result)
{
  output << std::setprecision(17);

  auto const time = result.samples.time_span();
  auto const sprung = result.samples.xs_span();
  auto const unsprung = result.samples.xu_span();
  auto const platform = result.samples.ground_span();
  auto const tire_force = result.samples.tire_force_span();

  output << "# table: raw\n"
            "time_s,sprung_displacement_m,unsprung_displacement_m,platform_displacement_m,tire_force_n\n";

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

  auto const refined_time = result.samples.time_span().subspan(result.measurement_offset, result.measurement_size);
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
          result.platform_signal.ampl,
          result.platform_signal.phase,
          result.platform_signal.freq,
          result.tire_force_signal.ampl,
          result.tire_force_signal.phase,
          result.tire_force_signal.freq))
  {
    output << time_value << ',' << platform_amplitude << ',' << platform_phase << ',' << platform_frequency << ','
           << tire_force_amplitude << ',' << tire_force_phase << ',' << tire_force_frequency << '\n';
  }
}

} // namespace hilbertcli
