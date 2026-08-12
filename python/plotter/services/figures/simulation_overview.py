import matplotlib.pyplot as plt
import numpy as np
from matplotlib.figure import Figure

from .common import MeasurementPlotData


def create_simulation_overview(plot_data: MeasurementPlotData) -> Figure:
    data = plot_data.simulation
    raw_samples = plot_data.raw_samples
    refined_samples = plot_data.refined_samples

    figure = plt.figure(figsize=(16, 9), layout="constrained")
    figure.canvas.manager.set_window_title("Hilbert simulation overview")
    raw_figure, refined_figure = figure.subfigures(
        1,
        2,
        width_ratios=(1, 1),
        wspace=0.08,
    )
    displacement_axis, raw_signal_axis = raw_figure.subplots(2, 1, sharex=True)
    frequency_axis, amplitude_axis, phase_axis = refined_figure.subplots(3, 1, sharex=True)

    line_width = 0.8

    displacement_axis.plot(
        plot_data.raw_time_s,
        data.raw["sprung_displacement_m"][raw_samples],
        label="Sprung mass",
        linewidth=line_width,
    )
    displacement_axis.plot(
        plot_data.raw_time_s,
        data.raw["unsprung_displacement_m"][raw_samples],
        label="Unsprung mass",
        linewidth=line_width,
    )
    displacement_axis.set_title("Mass displacements")
    displacement_axis.set_ylabel("Displacement (m)")
    displacement_axis.legend(loc="upper right")

    platform_line = raw_signal_axis.plot(
        plot_data.raw_time_s,
        data.raw["platform_displacement_m"][raw_samples],
        label="Platform",
        linewidth=line_width,
        color="tab:blue",
    )[0]
    tire_force_axis = raw_signal_axis.twinx()
    tire_force_line = tire_force_axis.plot(
        plot_data.raw_time_s,
        data.raw["tire_force_n"][raw_samples],
        label="Tire force",
        linewidth=line_width,
        color="tab:orange",
    )[0]
    raw_signal_axis.set_title("Platform position and tire force")
    raw_signal_axis.set_xlabel("Time (s)")
    raw_signal_axis.set_ylabel("Platform position (m)")
    raw_signal_axis.set_xlim(plot_data.measurement_start_s, plot_data.measurement_end_s)
    tire_force_axis.set_ylabel("Tire force (N)")
    raw_signal_axis.legend(handles=(platform_line, tire_force_line), loc="upper right")

    frequency_axis.plot(
        plot_data.refined_time_s,
        data.refined["platform_frequency_hz"][refined_samples],
        label="Platform",
        linewidth=line_width,
    )
    frequency_axis.plot(
        plot_data.refined_time_s,
        data.refined["tire_force_frequency_hz"][refined_samples],
        label="Tire force",
        linewidth=line_width,
    )
    frequency_axis.set_title("Instantaneous frequency")
    frequency_axis.set_ylabel("Frequency (Hz)")
    frequency_axis.legend(loc="upper right")

    platform_amplitude_line = amplitude_axis.plot(
        plot_data.refined_time_s,
        data.refined["platform_amplitude_m"][refined_samples],
        label="Platform",
        linewidth=line_width,
        color="tab:blue",
    )[0]
    tire_force_amplitude_axis = amplitude_axis.twinx()
    tire_force_amplitude_line = tire_force_amplitude_axis.plot(
        plot_data.refined_time_s,
        data.refined["tire_force_amplitude_n"][refined_samples],
        label="Tire force",
        linewidth=line_width,
        color="tab:orange",
    )[0]
    amplitude_axis.set_title("Instantaneous amplitude")
    amplitude_axis.set_ylabel("Platform amplitude (m)")
    tire_force_amplitude_axis.set_ylabel("Tire-force amplitude (N)")
    amplitude_axis.legend(
        handles=(platform_amplitude_line, tire_force_amplitude_line),
        loc="upper right",
    )

    phase_axis.plot(
        plot_data.refined_time_s,
        plot_data.phase_shift_rad,
        label="Platform relative to tire force",
        linewidth=line_width,
    )
    minimum_phase_shift_index = int(np.argmin(plot_data.phase_shift_rad))
    minimum_phase_shift = plot_data.phase_shift_rad[minimum_phase_shift_index]
    phase_axis.scatter(
        plot_data.refined_time_s[minimum_phase_shift_index],
        minimum_phase_shift,
        label=f"Minimum: {minimum_phase_shift:.3f} rad",
        color="tab:red",
        s=20,
        zorder=2,
    )
    phase_axis.set_title("Platform–tire-force phase shift")
    phase_axis.set_xlabel("Time (s)")
    phase_axis.set_ylabel("Phase shift (rad)")
    phase_axis.set_xlim(plot_data.measurement_start_s, plot_data.measurement_end_s)
    phase_axis.legend(loc="upper right")

    return figure
