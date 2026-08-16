import matplotlib.pyplot as plt
from matplotlib.figure import Figure

from ..frequency_sweep_data import FrequencySweepData


def create_frequency_sweep_phase_frequency_figure(data: FrequencySweepData) -> Figure:
    figure, axis = plt.subplots(figsize=(10, 7), layout="constrained")
    figure.canvas.manager.set_window_title("Constant-frequency phase shift sweep")

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
