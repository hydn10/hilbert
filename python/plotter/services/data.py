from collections.abc import Iterator
from dataclasses import dataclass
from pathlib import Path
from typing import TextIO

import numpy as np
from numpy.typing import NDArray

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
    "raw": RAW_COLUMNS,
    "refined": REFINED_COLUMNS,
}

StructuredArray = NDArray[np.void]


@dataclass(frozen=True)
class TimeSeriesData:
    data: StructuredArray
    sampling_frequency_hz: float

    def __getitem__(self, field: str) -> NDArray[np.float64]:
        return self.data[field]


@dataclass(frozen=True)
class SimulationData:
    raw: TimeSeriesData
    refined: TimeSeriesData


class _TableFormatError(ValueError):
    pass


class _TableParser:
    def __init__(self, source: TextIO) -> None:
        self._source = source
        self._line_number = 0
        self._pending_marker: tuple[str, int] | None = None
        self._rows_active = False

    def tables(self) -> Iterator[tuple[str, tuple[str, ...], Iterator[str]]]:
        while True:
            marker = self._take_marker()
            if marker is None:
                return

            table_name, marker_line = marker
            header_line = self._readline()
            if header_line == "":
                raise _TableFormatError(
                    f"table {table_name!r} at line {marker_line} is missing its header"
                )
            if not header_line.strip() or header_line.startswith("#"):
                raise _TableFormatError(
                    f"table {table_name!r} at line {marker_line} must have a CSV header "
                    "immediately after its marker"
                )

            header = tuple(header_line.rstrip("\r\n").split(","))
            if not all(header):
                raise _TableFormatError(
                    f"table {table_name!r} has a malformed header at line {self._line_number}"
                )

            self._rows_active = True
            yield table_name, header, self._table_rows()
            if self._rows_active:
                raise RuntimeError("table rows must be consumed before advancing to the next table")

    def _take_marker(self) -> tuple[str, int] | None:
        if self._pending_marker is not None:
            marker = self._pending_marker
            self._pending_marker = None
            return marker

        while True:
            line = self._readline()
            if line == "":
                return None
            if not line.strip():
                continue
            return self._parse_marker(line, self._line_number)

    def _table_rows(self) -> Iterator[str]:
        try:
            while True:
                line = self._readline()
                if line == "":
                    return
                if not line.strip():
                    continue
                if line.startswith("#"):
                    self._pending_marker = self._parse_marker(line, self._line_number)
                    return
                yield line
        finally:
            self._rows_active = False

    def _readline(self) -> str:
        line = self._source.readline()
        if line != "":
            self._line_number += 1
        return line

    @staticmethod
    def _parse_marker(line: str, line_number: int) -> tuple[str, int]:
        prefix = "# table: "
        marker = line.rstrip("\r\n")
        if not marker.startswith(prefix):
            raise _TableFormatError(f"expected '# table: <name>' at line {line_number}")
        table_name = marker[len(prefix) :].strip()
        if not table_name or any(character.isspace() for character in table_name):
            raise _TableFormatError(f"malformed table marker at line {line_number}")
        return table_name, line_number


def _load_table(table_name: str, rows: Iterator[str]) -> TimeSeriesData:
    columns = TABLE_SCHEMAS[table_name]
    dtype = np.dtype([(column, np.float64) for column in columns])
    try:
        data = np.loadtxt(rows, delimiter=",", dtype=dtype, ndmin=1)
    except _TableFormatError:
        raise
    except (TypeError, ValueError) as error:
        raise ValueError(f"table {table_name!r} contains invalid numeric data: {error}") from error

    if data.size < 2:
        raise ValueError(f"table {table_name!r} must contain at least two samples")

    for column in columns:
        if not np.all(np.isfinite(data[column])):
            raise ValueError(f"table {table_name!r} column {column!r} contains non-finite values")

    time_steps = np.diff(data["time_s"])
    if np.any(time_steps <= 0):
        raise ValueError(f"table {table_name!r} time_s must be strictly increasing")

    time_step = float(np.median(time_steps))
    if not np.allclose(time_steps, time_step, rtol=1e-6, atol=max(1e-12, time_step * 1e-9)):
        raise ValueError(f"table {table_name!r} time_s must use a uniform sampling interval")

    return TimeSeriesData(data=data, sampling_frequency_hz=1.0 / time_step)


def load_simulation_data(file_path: str | Path) -> SimulationData:
    """Load and validate Hilbert's multi-table simulation data contract."""
    path = Path(file_path)
    loaded_tables: dict[str, TimeSeriesData] = {}

    with path.open("r", encoding="utf-8", newline="") as source:
        for table_name, header, rows in _TableParser(source).tables():
            if table_name not in TABLE_SCHEMAS:
                raise ValueError(f"unknown table {table_name!r}")
            if table_name in loaded_tables:
                raise ValueError(f"duplicate table {table_name!r}")

            expected_header = TABLE_SCHEMAS[table_name]
            if header != expected_header:
                raise ValueError(
                    f"table {table_name!r} header must be {','.join(expected_header)}; "
                    f"got {','.join(header)}"
                )
            loaded_tables[table_name] = _load_table(table_name, rows)

    missing_tables = [name for name in TABLE_SCHEMAS if name not in loaded_tables]
    if missing_tables:
        raise ValueError(f"simulation data is missing required tables: {', '.join(missing_tables)}")

    return SimulationData(raw=loaded_tables["raw"], refined=loaded_tables["refined"])
