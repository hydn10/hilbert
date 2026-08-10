from dataclasses import dataclass
from pathlib import Path

import numpy as np
from numpy.typing import NDArray

REQUIRED_COLUMNS = (
    "time_s",
    "sprung_displacement_m",
    "unsprung_displacement_m",
    "platform_displacement_m",
    "tire_force_n",
    "platform_amplitude_m",
    "platform_phase_rad",
    "platform_frequency_hz",
    "tire_force_amplitude_n",
    "tire_force_phase_rad",
    "tire_force_frequency_hz",
)


@dataclass(frozen=True)
class SimulationData:
    columns: dict[str, NDArray[np.float64]]
    sampling_frequency_hz: float

    def __getitem__(self, field: str) -> NDArray[np.float64]:
        return self.columns[field]


def load_simulation_csv(file_path: str | Path) -> SimulationData:
    """Load and validate Hilbert's named-column simulation CSV contract."""
    path = Path(file_path)
    raw = np.genfromtxt(path, delimiter=",", names=True, dtype=np.float64, encoding="utf-8")

    column_names = raw.dtype.names or ()
    missing = sorted(set(REQUIRED_COLUMNS) - set(column_names))
    if missing:
        raise ValueError(f"simulation CSV is missing required columns: {', '.join(missing)}")

    columns = {name: np.atleast_1d(raw[name]) for name in REQUIRED_COLUMNS}
    time = columns["time_s"]
    if time.size < 2:
        raise ValueError("simulation CSV must contain at least two data rows")

    time_steps = np.diff(time)
    if not np.all(np.isfinite(time_steps)) or np.any(time_steps <= 0):
        raise ValueError("time_s must be finite and strictly increasing")

    time_step = float(np.median(time_steps))
    if not np.allclose(time_steps, time_step, rtol=1e-6, atol=max(1e-12, time_step * 1e-9)):
        raise ValueError("time_s must use a uniform sampling interval")

    for name, values in columns.items():
        if not np.all(np.isfinite(values)):
            raise ValueError(f"{name} contains non-finite values")

    return SimulationData(columns=columns, sampling_frequency_hz=1.0 / time_step)
