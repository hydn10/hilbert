from collections.abc import Iterator
from typing import TextIO

import numpy as np
from numpy.typing import NDArray

StructuredArray = NDArray[np.void]


class TableFormatError(ValueError):
    """Raised when a multi-table text file has invalid table framing."""


class TableParser:
    """Parse the project's generic ``# table: name`` text framing."""

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
                raise TableFormatError(
                    f"table {table_name!r} at line {marker_line} is missing its header"
                )
            if not header_line.strip() or header_line.startswith("#"):
                raise TableFormatError(
                    f"table {table_name!r} at line {marker_line} must have a CSV header "
                    "immediately after its marker"
                )

            header = tuple(header_line.rstrip("\r\n").split(","))
            if not all(header):
                raise TableFormatError(
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
            raise TableFormatError(f"expected '# table: <name>' at line {line_number}")
        table_name = marker[len(prefix) :].strip()
        if not table_name or any(character.isspace() for character in table_name):
            raise TableFormatError(f"malformed table marker at line {line_number}")
        return table_name, line_number


def load_numeric_rows(
    table_name: str,
    columns: tuple[str, ...],
    rows: Iterator[str],
) -> StructuredArray:
    """Load finite float columns from one parsed table."""
    dtype = np.dtype([(column, np.float64) for column in columns])
    try:
        data = np.loadtxt(rows, delimiter=",", dtype=dtype, ndmin=1)
    except TableFormatError:
        raise
    except (TypeError, ValueError) as error:
        raise ValueError(f"table {table_name!r} contains invalid numeric data: {error}") from error

    for column in columns:
        if not np.all(np.isfinite(data[column])):
            raise ValueError(f"table {table_name!r} column {column!r} contains non-finite values")

    return data
