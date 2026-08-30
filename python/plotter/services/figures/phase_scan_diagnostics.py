from __future__ import annotations

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.figure import Figure
from matplotlib.ticker import PercentFormatter

from ..phase_scan_data import PhaseScanData


def _finite_maximum(values: np.ndarray) -> float | None:
    finite_values = values[np.isfinite(values)]
    if finite_values.size == 0:
        return None
    return float(np.max(finite_values))


def _nice_linear_upper_limit(values: np.ndarray, minimum: float) -> float:
    """Return a rounded, zero-based upper limit for a linear diagnostic axis."""
    maximum = _finite_maximum(values)
    if maximum is None or maximum <= minimum:
        return minimum

    exponent = np.floor(np.log10(maximum))
    magnitude = 10.0**exponent
    for multiplier in (1.0, 2.0, 5.0, 10.0):
        limit = multiplier * magnitude
        if limit >= maximum:
            return limit

    return 10.0 * magnitude


def _condition_upper_limit(values: np.ndarray) -> float:
    """Return a power-of-ten upper limit for the log-scaled condition axis."""
    maximum = _finite_maximum(values)
    if maximum is None or maximum <= 10.0:
        return 10.0
    return 10.0 ** np.ceil(np.log10(maximum))


def _upper_edge_values(values: np.ndarray, upper_limit: float) -> tuple[np.ndarray, np.ndarray]:
    """Replace positive infinities with an explicit, plottable upper-edge value."""
    plotted_values = np.array(values, dtype=float, copy=True)
    upper_edge = np.isposinf(plotted_values)
    plotted_values[upper_edge] = upper_limit
    return plotted_values, upper_edge


def _add_ideal_reference_line(axis, value: float) -> None:
    axis.axhline(
        value,
        color="0.45",
        linestyle=":",
        linewidth=0.8,
        alpha=0.7,
        zorder=1,
    )


def _mark_inadmissible_lower_region(axis, physical_lower_bound: float) -> None:
    """Subtly mark any displayed y-range below the diagnostic's valid domain."""
    lower_limit, upper_limit = axis.get_ylim()
    if lower_limit >= physical_lower_bound or upper_limit <= physical_lower_bound:
        return

    axis.axhspan(
        lower_limit,
        physical_lower_bound,
        facecolor="0.96",
        edgecolor="0.80",
        hatch="/",
        linewidth=0.4,
        alpha=0.65,
        zorder=0,
    )


def _set_padded_diagnostic_ylim(
    axis,
    physical_lower_bound: float,
    upper_limit: float,
    padding_fraction: float = 0.05,
) -> None:
    """Leave a small marker cushion below the physical lower bound."""
    if physical_lower_bound == 0.0:
        lower_limit = -padding_fraction * upper_limit
    else:
        lower_limit = physical_lower_bound / (1.0 + padding_fraction)

    axis.set_ylim(lower_limit, upper_limit)
    _mark_inadmissible_lower_region(axis, physical_lower_bound)


def _phase_dispersion_degrees(resultant_length: np.ndarray) -> np.ndarray:
    """Convert mean resultant length to circular phase dispersion in degrees."""
    bounded_resultant = np.clip(np.asarray(resultant_length, dtype=float), 0.0, 1.0)
    with np.errstate(divide="ignore", invalid="ignore"):
        dispersion_squared = np.maximum(0.0, -2.0 * np.log(bounded_resultant))
        dispersion_radians = np.sqrt(dispersion_squared)
    return np.degrees(dispersion_radians)


def create_phase_scan_diagnostics_figure(data: PhaseScanData) -> Figure:
    figure, axes = plt.subplots(
        4,
        1,
        sharex=True,
        figsize=(10, 12),
        layout="constrained",
    )
    figure.canvas.manager.set_window_title("Constant-frequency response diagnostics")

    condition_axis, residual_axis, coherence_axis, magnitude_axis = axes
    frequency = data["frequency_hz"]

    condition_upper_limit = _condition_upper_limit(data["least_squares_basis_condition_number"])
    condition_values, singular_condition = _upper_edge_values(
        data["least_squares_basis_condition_number"], condition_upper_limit
    )
    (condition_line,) = condition_axis.plot(
        frequency,
        condition_values,
        label="Least-squares basis condition number",
        marker="o",
    )
    condition_axis.set_title("Least-squares sinusoidal basis conditioning")
    condition_axis.set_ylabel("Condition number (kappa)")
    condition_axis.set_yscale("log")
    _set_padded_diagnostic_ylim(condition_axis, 1.0, condition_upper_limit, padding_fraction=0.10)
    _add_ideal_reference_line(condition_axis, 1.0)
    if np.any(singular_condition):
        condition_axis.plot(
            frequency[singular_condition],
            condition_values[singular_condition],
            linestyle="none",
            marker="v",
            markersize=7,
            color=condition_line.get_color(),
            markeredgecolor="white",
            markeredgewidth=0.7,
            clip_on=False,
            label="Singular basis (condition = infinite)",
        )
    condition_axis.legend(loc="best")

    least_squares_residuals = 100.0 * np.concatenate(
        (
            data["least_squares_ground_normalized_residual"],
            data["least_squares_tire_force_normalized_residual"],
        )
    )
    residual_axis.plot(
        frequency,
        100.0 * data["least_squares_ground_normalized_residual"],
        label="Ground displacement residual",
        marker="o",
    )
    residual_axis.plot(
        frequency,
        100.0 * data["least_squares_tire_force_normalized_residual"],
        label="Tire-force residual",
        marker="x",
    )
    residual_axis.set_title("Least-squares normalized fit residuals")
    residual_axis.set_ylabel("Normalized residual (%)")
    residual_upper_limit = _nice_linear_upper_limit(least_squares_residuals, 1.0)
    _set_padded_diagnostic_ylim(residual_axis, 0.0, residual_upper_limit)
    residual_axis.yaxis.set_major_formatter(PercentFormatter(xmax=100.0))
    _add_ideal_reference_line(residual_axis, 0.0)
    residual_axis.legend(loc="best")

    phase_dispersion = _phase_dispersion_degrees(data["hilbert_mean_resultant_length"])
    phase_upper_limit = _nice_linear_upper_limit(phase_dispersion, 10.0)
    phase_dispersion, unbounded_dispersion = _upper_edge_values(phase_dispersion, phase_upper_limit)
    (phase_line,) = coherence_axis.plot(
        frequency,
        phase_dispersion,
        label="Hilbert relative-phase dispersion",
        marker="o",
    )
    coherence_axis.set_title("Hilbert relative-phase dispersion")
    coherence_axis.set_ylabel("Phase dispersion (degrees)")
    _set_padded_diagnostic_ylim(coherence_axis, 0.0, phase_upper_limit)
    if np.any(unbounded_dispersion):
        coherence_axis.plot(
            frequency[unbounded_dispersion],
            phase_dispersion[unbounded_dispersion],
            linestyle="none",
            marker="v",
            markersize=7,
            color=phase_line.get_color(),
            markeredgecolor="white",
            markeredgewidth=0.7,
            clip_on=False,
            label="Unbounded dispersion (resultant length = 0)",
        )
    _add_ideal_reference_line(coherence_axis, 0.0)
    coherence_axis.legend(loc="best")

    magnitude_residual = 100.0 * data["hilbert_magnitude_normalized_residual"]
    magnitude_axis.plot(
        frequency,
        magnitude_residual,
        label="Hilbert magnitude normalized residual",
        marker="o",
    )
    magnitude_axis.set_title("Hilbert envelope proportionality")
    magnitude_axis.set_xlabel("Ground frequency (Hz)")
    magnitude_axis.set_ylabel("Envelope residual (%)")
    magnitude_upper_limit = _nice_linear_upper_limit(magnitude_residual, 1.0)
    _set_padded_diagnostic_ylim(magnitude_axis, 0.0, magnitude_upper_limit)
    magnitude_axis.yaxis.set_major_formatter(PercentFormatter(xmax=100.0))
    _add_ideal_reference_line(magnitude_axis, 0.0)
    magnitude_axis.legend(loc="best")

    return figure
