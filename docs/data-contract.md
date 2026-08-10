# Simulation CSV contract

`hilbert-cli` is the authoritative producer of the file interface between the native simulation and the Python plotter. It writes RFC-style comma-separated data with one header row and numeric data rows. The plotter resolves fields by name, never by position, and derives the sampling frequency from `time_s`.

| Column                    | Unit | Meaning                                                              |
| ------------------------- | ---- | -------------------------------------------------------------------- |
| `time_s`                  | s    | Simulation time; finite, strictly increasing, and uniformly sampled. |
| `sprung_displacement_m`   | m    | Sprung-mass displacement, $x_s$.                                     |
| `unsprung_displacement_m` | m    | Unsprung-mass displacement, $x_u$.                                   |
| `platform_displacement_m` | m    | Test-platform displacement, $y_g$.                                   |
| `tire_force_n`            | N    | Tire force, $k_t(x_u-y_g)$.                                          |
| `platform_amplitude_m`    | m    | Instantaneous platform-signal amplitude.                             |
| `platform_phase_rad`      | rad  | Wrapped instantaneous platform-signal phase.                         |
| `platform_frequency_hz`   | Hz   | Instantaneous platform-signal frequency.                             |
| `tire_force_amplitude_n`  | N    | Instantaneous tire-force-signal amplitude.                           |
| `tire_force_phase_rad`    | rad  | Wrapped instantaneous tire-force-signal phase.                       |
| `tire_force_frequency_hz` | Hz   | Instantaneous tire-force-signal frequency.                           |

All fields are finite decimal numbers. Consumers may accept additional named columns, but these fields are required. The CTest schema check and Python loader tests make removal or renaming an explicit, reviewed change.
