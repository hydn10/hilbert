#ifndef HILBERT_EGEA_OUTPUT_HPP
#define HILBERT_EGEA_OUTPUT_HPP


#include <hilbert/app/io/numeric_table_writer.hpp>
#include <hilbert/egea/simulation.hpp>
#include <hilbert/hilbert.hpp>

#include <array>
#include <ostream>
#include <ranges>
#include <string_view>


namespace hilbert::egea
{

template<hilbert::supported_float Float>
void
write_simulation_data(
    std::ostream &output, simulation_data<Float> const &samples, analysis_result<Float> const &analysis);


template<hilbert::supported_float Float>
void
write_simulation_data(
    std::ostream &output, simulation_data<Float> const &samples, analysis_result<Float> const &analysis)
{
  hilbert::app::io::numeric_table_writer<Float> document{output};

  constexpr std::array intervals_columns{
      std::string_view{"measurement_start_s"},
      std::string_view{"measurement_end_s"},
      std::string_view{"hilbert_start_s"},
      std::string_view{"hilbert_end_s"},
  };

  document.table(
      "intervals",
      intervals_columns,
      [&analysis](auto &rows)
      {
        rows.write(
            analysis.measurement_interval.start_time,
            analysis.measurement_interval.end_time,
            analysis.hilbert_interval.start_time,
            analysis.hilbert_interval.end_time);
      });

  constexpr std::array raw_columns{
      std::string_view{"time_s"},
      std::string_view{"sprung_displacement_m"},
      std::string_view{"unsprung_displacement_m"},
      std::string_view{"platform_displacement_m"},
      std::string_view{"tire_force_n"},
  };

  auto const time = samples.time_span();
  auto const sprung = samples.sprung_displacement_span();
  auto const unsprung = samples.unsprung_displacement_span();
  auto const platform = samples.ground_displacement_span();
  auto const tire_force = samples.tire_force_span();

  document.table(
      "raw",
      raw_columns,
      [&](auto &rows)
      {
        for (
            auto const &[time_value, sprung_value, unsprung_value, platform_value, tire_force_value] :
            std::views::zip(time, sprung, unsprung, platform, tire_force))
        {
          rows.write(time_value, sprung_value, unsprung_value, platform_value, tire_force_value);
        }
      });

  constexpr std::array refined_columns{
      std::string_view{"time_s"},
      std::string_view{"platform_amplitude_m"},
      std::string_view{"platform_phase_rad"},
      std::string_view{"platform_frequency_hz"},
      std::string_view{"tire_force_amplitude_n"},
      std::string_view{"tire_force_phase_rad"},
      std::string_view{"tire_force_frequency_hz"},
  };

  auto const refined_time = samples.time_span().subspan(analysis.hilbert_offset, analysis.hilbert_size);

  document.table(
      "refined",
      refined_columns,
      [&](auto &rows)
      {
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
          rows.write(
              time_value,
              platform_amplitude,
              platform_phase,
              platform_frequency,
              tire_force_amplitude,
              tire_force_phase,
              tire_force_frequency);
        }
      });
}

} // namespace hilbert::egea

#endif // HILBERT_EGEA_OUTPUT_HPP
