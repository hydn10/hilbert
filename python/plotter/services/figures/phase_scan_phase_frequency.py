import matplotlib.pyplot as plt
from matplotlib.figure import Figure

from ..phase_scan_data import PhaseScanData


def create_phase_scan_phase_frequency_figure(data: PhaseScanData) -> Figure:
    figure, axis = plt.subplots(figsize=(10, 7), layout="constrained")
    figure.canvas.manager.set_window_title("Constant-frequency phase scan")

    axis.plot(
        data["frequency_hz"],
        data["phase_fit_rad"],
        label="Least-squares phase",
        marker="o",
    )
    axis.plot(
        data["frequency_hz"],
        data["phase_hilbert_rad"],
        label="Hilbert phase",
        marker="x",
    )

    axis.set_title("Estimated phase shift versus constant ground frequency")
    axis.set_xlabel("Ground frequency (Hz)")
    axis.set_ylabel("Phase shift (rad)")
    axis.legend(loc="best")

    return figure
