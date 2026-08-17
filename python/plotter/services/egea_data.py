from dataclasses import dataclass
from pathlib import Path

import numpy as np
from numpy.typing import NDArray

from .tables import StructuredArray, load_numeric_tables

INTERVAL_COLUMNS = (
    "measurement_start_s",
    "measurement_end_s",
    "hilbert_start_s",
    "hilbert_end_s",
)

RAW_COLUMNS = (
    "time_s",
    "sprung_displacement_m",
    "unsprung_displacement_m",
    "platform_displacement_m",
    "tire_force_n",
)

REFINED_COLUMNS = (
    "time_s",
    "platform_amplitude_m",
    "platform_phase_rad",
    "platform_frequency_hz",
    "tire_force_amplitude_n",
    "tire_force_phase_rad",
    "tire_force_frequency_hz",
)

TABLE_SCHEMAS = {
    "intervals": INTERVAL_COLUMNS,
    "raw": RAW_COLUMNS,
    "refined": REFINED_COLUMNS,
}


@dataclass(frozen=True)
class EgeaTimeSeriesData:
    data: StructuredArray
    sampling_frequency_hz: float

    def __getitem__(self, field: str) -> NDArray[np.float64]:
        return self.data[field]


@dataclass(frozen=True)
class EgeaIntervals:
    measurement_start_s: float
    measurement_end_s: float
    hilbert_start_s: float
    hilbert_end_s: float


@dataclass(frozen=True)
class EgeaData:
    intervals: EgeaIntervals
    raw: EgeaTimeSeriesData
    refined: EgeaTimeSeriesData


def _load_time_series_table(table_name: str, data: StructuredArray) -> EgeaTimeSeriesData:
    if data.size < 2:
        raise ValueError(f"table {table_name!r} must contain at least two samples")

    time_steps = np.diff(data["time_s"])
    if np.any(time_steps <= 0):
        raise ValueError(f"table {table_name!r} time_s must be strictly increasing")

    time_step = float(np.median(time_steps))
    if not np.allclose(time_steps, time_step, rtol=1e-6, atol=max(1e-12, time_step * 1e-9)):
        raise ValueError(f"table {table_name!r} time_s must use a uniform sampling interval")

    return EgeaTimeSeriesData(data=data, sampling_frequency_hz=1.0 / time_step)


def _load_intervals(data: StructuredArray) -> EgeaIntervals:
    if data.size != 1:
        raise ValueError("table 'intervals' must contain exactly one row")

    intervals = EgeaIntervals(
        measurement_start_s=float(data["measurement_start_s"][0]),
        measurement_end_s=float(data["measurement_end_s"][0]),
        hilbert_start_s=float(data["hilbert_start_s"][0]),
        hilbert_end_s=float(data["hilbert_end_s"][0]),
    )
    if not (
        intervals.hilbert_start_s
        <= intervals.measurement_start_s
        < intervals.measurement_end_s
        <= intervals.hilbert_end_s
    ):
        raise ValueError(
            "table 'intervals' must satisfy "
            "hilbert_start_s <= measurement_start_s < measurement_end_s <= hilbert_end_s"
        )

    return intervals


def load_egea_data(file_path: str | Path) -> EgeaData:
    """Load and validate the EGEA multi-table simulation data contract."""
    path = Path(file_path)

    with path.open("r", encoding="utf-8", newline="") as source:
        loaded_tables = load_numeric_tables(source, TABLE_SCHEMAS, document_name="simulation data")

    intervals = _load_intervals(loaded_tables["intervals"])
    raw = _load_time_series_table("raw", loaded_tables["raw"])
    refined = _load_time_series_table("refined", loaded_tables["refined"])

    for table_name in ("raw", "refined"):
        time_series = raw if table_name == "raw" else refined
        time = time_series["time_s"]
        measurement_samples = (time >= intervals.measurement_start_s) & (
            time < intervals.measurement_end_s
        )
        if np.count_nonzero(measurement_samples) < 2:
            raise ValueError(
                f"table {table_name!r} must contain at least two samples in the measurement interval"
            )

    refined_time = refined["time_s"]
    if np.any(refined_time < intervals.hilbert_start_s) or np.any(
        refined_time >= intervals.hilbert_end_s
    ):
        raise ValueError("table 'refined' contains samples outside the Hilbert interval")

    return EgeaData(
        intervals=intervals,
        raw=raw,
        refined=refined,
    )
