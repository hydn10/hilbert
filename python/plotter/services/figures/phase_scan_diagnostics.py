import matplotlib.pyplot as plt
from matplotlib.figure import Figure

from ..phase_scan_data import PhaseScanData


def create_phase_scan_diagnostics_figure(data: PhaseScanData) -> Figure:
    figure, axes = plt.subplots(
        4,
        1,
        sharex=True,
        figsize=(10, 12),
        layout="constrained",
    )
    figure.canvas.manager.set_window_title("Constant-frequency response diagnostics")

    condition_axis, residual_axis, coherence_axis, gain_axis = axes
    frequency = data["frequency_hz"]

    condition_axis.plot(
        frequency,
        data["least_squares_basis_condition_number"],
        label="Least-squares basis condition number",
        marker="o",
    )
    condition_axis.set_title("Least-squares sinusoidal basis conditioning")
    condition_axis.set_ylabel("Condition number")
    condition_axis.set_yscale("log")
    condition_axis.legend(loc="best")

    residual_axis.plot(
        frequency,
        data["least_squares_ground_normalized_residual"],
        label="Ground displacement residual",
        marker="o",
    )
    residual_axis.plot(
        frequency,
        data["least_squares_tire_force_normalized_residual"],
        label="Tire-force residual",
        marker="x",
    )
    residual_axis.set_title("Least-squares normalized fit residuals")
    residual_axis.set_ylabel("Residual / signal variation")
    residual_axis.legend(loc="best")

    coherence_axis.plot(
        frequency,
        data["hilbert_mean_resultant_length"],
        label="Hilbert mean resultant length",
        marker="o",
    )
    coherence_axis.set_title("Hilbert relative-phase coherence")
    coherence_axis.set_ylabel("Mean resultant length")
    coherence_axis.set_ylim(0, 1)
    coherence_axis.legend(loc="best")

    gain_axis.plot(
        frequency,
        data["hilbert_gain_coefficient_of_variation"],
        label="Hilbert gain coefficient of variation",
        marker="o",
    )
    gain_axis.set_title("Hilbert instantaneous gain stability")
    gain_axis.set_xlabel("Ground frequency (Hz)")
    gain_axis.set_ylabel("Gain CV")
    gain_axis.legend(loc="best")

    return figure
