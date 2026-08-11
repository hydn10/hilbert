from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, filtfilt, hilbert

from .data import load_simulation_data


def continuous_relative_phase(
    first_analytic_signal: np.ndarray,
    second_analytic_signal: np.ndarray,
) -> np.ndarray:
    relative_analytic_signal = first_analytic_signal * np.conj(second_analytic_signal)
    return np.unwrap(np.angle(relative_analytic_signal))


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
    platform = data.raw["platform_displacement_m"]
    tire_force = data.raw["tire_force_n"]

    filtered_force = bandpass_filter(tire_force, 5, 30, data.raw.sampling_frequency_hz)
    filtered_platform = bandpass_filter(platform, 2, 22, data.raw.sampling_frequency_hz)

    platform_analytic = data.refined["platform_amplitude_m"] * np.exp(
        1j * data.refined["platform_phase_rad"]
    )
    force_analytic = data.refined["tire_force_amplitude_n"] * np.exp(
        1j * data.refined["tire_force_phase_rad"]
    )
    native_phase_shift = continuous_relative_phase(platform_analytic, force_analytic)

    filtered_platform_analytic = hilbert(filtered_platform)
    filtered_force_analytic = hilbert(filtered_force)
    filtered_phase_shift = continuous_relative_phase(
        filtered_platform_analytic,
        filtered_force_analytic,
    )

    figure, axes = plt.subplots(2, 1, sharex=True, figsize=(10, 8))
    line_width = 0.8

    axes[0].plot(
        refined_time, native_phase_shift, label="Native analytic signals", linewidth=line_width
    )
    axes[0].set_title("Platform–tire-force phase shift")
    axes[0].set_ylabel("Phase shift (rad)")
    axes[0].legend(loc="upper right")

    axes[1].plot(
        raw_time, filtered_phase_shift, label="SciPy filtered signals", linewidth=line_width
    )
    axes[1].set_title("Band-pass-filtered phase shift")
    axes[1].set_xlabel("Time (s)")
    axes[1].set_ylabel("Phase shift (rad)")
    axes[1].legend(loc="upper right")

    figure.tight_layout()

    if save_path is not None:
        figure.savefig(save_path)
    if show:
        plt.show()
    plt.close(figure)
