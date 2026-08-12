from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, filtfilt

from .data import load_simulation_data


def positive_continuous_relative_phase(
    first_phase: np.ndarray,
    second_phase: np.ndarray,
) -> np.ndarray:
    wrapped_relative_phase = np.angle(np.exp(1j * (first_phase - second_phase)))
    relative_phase = np.unwrap(wrapped_relative_phase)

    full_turn = 2 * np.pi
    full_turns_to_add = max(
        0,
        int(np.floor(-np.min(relative_phase) / full_turn)) + 1,
    )
    return relative_phase + full_turns_to_add * full_turn


def bandpass_filter(
    signal: np.ndarray,
    low_cut_hz: float,
    high_cut_hz: float,
    sampling_frequency_hz: float,
    order: int = 3,
) -> np.ndarray:
    nyquist_frequency = 0.5 * sampling_frequency_hz
    if not 0 < low_cut_hz < high_cut_hz < nyquist_frequency:
        raise ValueError("band-pass cutoffs must lie between zero and the Nyquist frequency")
    numerator, denominator = butter(
        order,
        [low_cut_hz / nyquist_frequency, high_cut_hz / nyquist_frequency],
        btype="band",
    )
    return filtfilt(numerator, denominator, signal)


def plot_data(
    file_path: str | Path,
    *,
    save_path: str | Path | None = None,
    show: bool = True,
) -> None:
    data = load_simulation_data(file_path)

    raw_time = data.raw["time_s"]
    refined_time = data.refined["time_s"]
    platform_tire_force_phase_shift = positive_continuous_relative_phase(
        data.refined["platform_phase_rad"],
        data.refined["tire_force_phase_rad"],
    )

    figure = plt.figure(figsize=(16, 9), layout="constrained")
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
        raw_time,
        data.raw["sprung_displacement_m"],
        label="Sprung mass",
        linewidth=line_width,
    )
    displacement_axis.plot(
        raw_time,
        data.raw["unsprung_displacement_m"],
        label="Unsprung mass",
        linewidth=line_width,
    )
    displacement_axis.set_title("Mass displacements")
    displacement_axis.set_ylabel("Displacement (m)")
    displacement_axis.legend(loc="upper right")

    ground_line = raw_signal_axis.plot(
        raw_time,
        data.raw["platform_displacement_m"],
        label="Ground position",
        linewidth=line_width,
        color="tab:blue",
    )[0]
    tire_force_axis = raw_signal_axis.twinx()
    tire_force_line = tire_force_axis.plot(
        raw_time,
        data.raw["tire_force_n"],
        label="Tire force",
        linewidth=line_width,
        color="tab:orange",
    )[0]
    raw_signal_axis.set_title("Ground position and tire force")
    raw_signal_axis.set_xlabel("Time (s)")
    raw_signal_axis.set_ylabel("Ground position (m)")
    tire_force_axis.set_ylabel("Tire force (N)")
    raw_signal_axis.legend(handles=(ground_line, tire_force_line), loc="upper right")

    frequency_axis.plot(
        refined_time,
        data.refined["platform_frequency_hz"],
        label="Platform",
        linewidth=line_width,
    )
    frequency_axis.plot(
        refined_time,
        data.refined["tire_force_frequency_hz"],
        label="Tire force",
        linewidth=line_width,
    )
    frequency_axis.set_title("Instantaneous frequency")
    frequency_axis.set_ylabel("Frequency (Hz)")
    frequency_axis.legend(loc="upper right")

    platform_amplitude_line = amplitude_axis.plot(
        refined_time,
        data.refined["platform_amplitude_m"],
        label="Platform",
        linewidth=line_width,
        color="tab:blue",
    )[0]
    tire_force_amplitude_axis = amplitude_axis.twinx()
    tire_force_amplitude_line = tire_force_amplitude_axis.plot(
        refined_time,
        data.refined["tire_force_amplitude_n"],
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
        refined_time,
        platform_tire_force_phase_shift,
        label="Platform relative to tire force",
        linewidth=line_width,
    )
    minimum_phase_shift_index = int(np.argmin(platform_tire_force_phase_shift))
    minimum_phase_shift = platform_tire_force_phase_shift[minimum_phase_shift_index]
    phase_axis.scatter(
        refined_time[minimum_phase_shift_index],
        minimum_phase_shift,
        label=f"Minimum: {minimum_phase_shift:.3f} rad",
        color="tab:red",
        s=20,
        zorder=2,
    )
    phase_axis.set_title("Platform–tire-force phase shift")
    phase_axis.set_xlabel("Time (s)")
    phase_axis.set_ylabel("Phase shift (rad)")
    phase_axis.legend(loc="upper right")

    if save_path is not None:
        figure.savefig(save_path)
    if show:
        plt.show()
    plt.close(figure)
