import matplotlib.pyplot as plt
import numpy as np
from matplotlib.figure import Figure

from .common import MeasurementPlotData


def create_phase_frequency_figure(plot_data: MeasurementPlotData) -> Figure:
    figure, axis = plt.subplots(figsize=(10, 7), layout="constrained")
    figure.canvas.manager.set_window_title("Phase shift versus platform frequency")

    line_width = 0.8
    axis.plot(
        plot_data.platform_frequency_hz,
        plot_data.phase_shift_rad,
        label="Platform relative to tire force",
        linewidth=line_width,
    )

    minimum_phase_shift_index = int(np.argmin(plot_data.phase_shift_rad))
    minimum_phase_shift = plot_data.phase_shift_rad[minimum_phase_shift_index]
    minimum_frequency = plot_data.platform_frequency_hz[minimum_phase_shift_index]
    axis.scatter(
        minimum_frequency,
        minimum_phase_shift,
        label=f"Minimum: {minimum_phase_shift:.3f} rad at {minimum_frequency:.3f} Hz",
        color="tab:red",
        s=20,
        zorder=2,
    )

    axis.set_title("Platform–tire-force phase shift versus platform frequency")
    axis.set_xlabel("Platform instantaneous frequency (Hz)")
    axis.set_ylabel("Phase shift (rad)")
    axis.legend(loc="upper right")

    return figure
