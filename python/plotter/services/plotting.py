from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, filtfilt, hilbert

from .data import load_simulation_csv


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
    data = load_simulation_csv(file_path)

    time = data["time_s"]
    platform = data["platform_displacement_m"]
    tire_force = data["tire_force_n"]

    filtered_force = bandpass_filter(tire_force, 5, 30, data.sampling_frequency_hz)
    filtered_platform = bandpass_filter(platform, 2, 22, data.sampling_frequency_hz)

    native_phase_shift = np.unwrap(data["platform_phase_rad"]) - np.unwrap(
        data["tire_force_phase_rad"]
    )
    filtered_phase_shift = np.unwrap(np.angle(hilbert(filtered_platform))) - np.unwrap(
        np.angle(hilbert(filtered_force))
    )

    figure, axes = plt.subplots(2, 1, sharex=True, figsize=(10, 8))
    line_width = 0.8

    axes[0].plot(time, native_phase_shift, label="Native analytic signals", linewidth=line_width)
    axes[0].set_title("Platform–tire-force phase shift")
    axes[0].set_ylabel("Phase shift (rad)")
    axes[0].legend(loc="upper right")

    axes[1].plot(time, filtered_phase_shift, label="SciPy filtered signals", linewidth=line_width)
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
