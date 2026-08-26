from dataclasses import dataclass
from pathlib import Path

import numpy as np
from numpy.typing import NDArray

from .tables import StructuredArray, load_numeric_tables

PHASE_SCAN_RESULT_COLUMNS = (
    "frequency_hz",
    "magnitude_least_squares_n_per_m",
    "magnitude_hilbert_n_per_m",
    "phase_least_squares_rad",
    "phase_hilbert_rad",
    "least_squares_basis_condition_number",
    "least_squares_ground_normalized_residual",
    "least_squares_tire_force_normalized_residual",
    "hilbert_mean_resultant_length",
    "hilbert_gain_coefficient_of_variation",
)


@dataclass(frozen=True)
class PhaseScanData:
    data: StructuredArray

    def __getitem__(self, field: str) -> NDArray[np.float64]:
        return self.data[field]


def load_phase_scan_data(file_path: str | Path) -> PhaseScanData:
    """Load and validate the phase-scan response and diagnostics contract."""
    path = Path(file_path)

    with path.open("r", encoding="utf-8", newline="") as source:
        results = load_numeric_tables(
            source,
            {"results": PHASE_SCAN_RESULT_COLUMNS},
            document_name="phase-scan data",
        )["results"]

    if results.size == 0:
        raise ValueError("table 'results' must contain at least one row")

    frequencies = results["frequency_hz"]
    if np.any(frequencies <= 0):
        raise ValueError("table 'results' frequency_hz must be positive")
    if results.size > 1 and np.any(np.diff(frequencies) <= 0):
        raise ValueError("table 'results' frequency_hz must be strictly increasing")

    for column in (
        "magnitude_least_squares_n_per_m",
        "magnitude_hilbert_n_per_m",
    ):
        if np.any(results[column] < 0):
            raise ValueError(f"table 'results' {column} must be non-negative")

    if np.any(results["least_squares_basis_condition_number"] < 1):
        raise ValueError(
            "table 'results' least_squares_basis_condition_number must be at least one"
        )

    for column in (
        "least_squares_ground_normalized_residual",
        "least_squares_tire_force_normalized_residual",
        "hilbert_gain_coefficient_of_variation",
    ):
        if np.any(results[column] < 0):
            raise ValueError(f"table 'results' {column} must be non-negative")

    if np.any(
        (results["hilbert_mean_resultant_length"] < 0)
        | (results["hilbert_mean_resultant_length"] > 1)
    ):
        raise ValueError("table 'results' hilbert_mean_resultant_length must be in [0, 1]")

    return PhaseScanData(data=results)
