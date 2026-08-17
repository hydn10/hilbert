from dataclasses import dataclass
from pathlib import Path

import numpy as np
from numpy.typing import NDArray

from .tables import StructuredArray, TableParser, load_numeric_rows

PHASE_SCAN_RESULT_COLUMNS = (
    "frequency_hz",
    "phase_fit_rad",
    "phase_hilbert_rad",
)


@dataclass(frozen=True)
class PhaseScanData:
    data: StructuredArray

    def __getitem__(self, field: str) -> NDArray[np.float64]:
        return self.data[field]


def load_phase_scan_data(file_path: str | Path) -> PhaseScanData:
    """Load and validate the phase-scan results contract."""
    path = Path(file_path)
    results: StructuredArray | None = None
    loaded_table = False

    with path.open("r", encoding="utf-8", newline="") as source:
        for table_name, header, rows in TableParser(source).tables():
            if table_name != "results":
                raise ValueError(f"unknown table {table_name!r}")
            if loaded_table:
                raise ValueError("duplicate table 'results'")
            loaded_table = True

            if header != PHASE_SCAN_RESULT_COLUMNS:
                raise ValueError(
                    f"table 'results' header must be {','.join(PHASE_SCAN_RESULT_COLUMNS)}; "
                    f"got {','.join(header)}"
                )
            results = load_numeric_rows("results", PHASE_SCAN_RESULT_COLUMNS, rows)

    if results is None:
        raise ValueError("phase-scan data is missing required table: results")
    if results.size == 0:
        raise ValueError("table 'results' must contain at least one row")

    frequencies = results["frequency_hz"]
    if np.any(frequencies <= 0):
        raise ValueError("table 'results' frequency_hz must be positive")
    if results.size > 1 and np.any(np.diff(frequencies) <= 0):
        raise ValueError("table 'results' frequency_hz must be strictly increasing")

    return PhaseScanData(data=results)
