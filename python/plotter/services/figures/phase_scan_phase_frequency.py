import matplotlib.pyplot as plt
from matplotlib.figure import Figure

from ..phase_scan_data import PhaseScanData


def create_phase_scan_phase_frequency_figure(data: PhaseScanData) -> Figure:
    figure, (magnitude_axis, phase_axis) = plt.subplots(
        2,
        1,
        sharex=True,
        figsize=(10, 9),
        layout="constrained",
    )
    figure.canvas.manager.set_window_title("Constant-frequency response scan")

    magnitude_axis.plot(
        data["frequency_hz"],
        data["magnitude_fit_n_per_m"],
        label="Least-squares magnitude",
        marker="o",
    )
    magnitude_axis.plot(
        data["frequency_hz"],
        data["magnitude_hilbert_n_per_m"],
        label="Hilbert magnitude",
        marker="x",
    )
    magnitude_axis.set_title("Estimated tire-force / ground-displacement magnitude")
    magnitude_axis.set_ylabel("Magnitude ratio (N/m)")
    magnitude_axis.legend(loc="best")

    phase_axis.plot(
        data["frequency_hz"],
        data["phase_fit_rad"],
        label="Least-squares phase",
        marker="o",
    )
    phase_axis.plot(
        data["frequency_hz"],
        data["phase_hilbert_rad"],
        label="Hilbert phase",
        marker="x",
    )
    phase_axis.set_title("Estimated tire-force / ground-displacement phase")
    phase_axis.set_xlabel("Ground frequency (Hz)")
    phase_axis.set_ylabel("Force phase - ground phase (rad)")
    phase_axis.legend(loc="best")

    return figure
