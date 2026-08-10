from pathlib import Path

import pytest
from plotter.services.data import REQUIRED_COLUMNS, load_simulation_csv


def write_csv(path: Path, header: tuple[str, ...], rows: list[tuple[float, ...]]) -> None:
    path.write_text(
        ",".join(header) + "\n" + "\n".join(",".join(map(str, row)) for row in rows) + "\n",
        encoding="utf-8",
    )


def complete_row(time: float) -> tuple[float, ...]:
    return (time,) + (0.0,) * (len(REQUIRED_COLUMNS) - 1)


def test_loads_named_columns_and_derives_sampling_frequency(tmp_path: Path) -> None:
    path = tmp_path / "simulation.csv"
    write_csv(
        path, REQUIRED_COLUMNS, [complete_row(0.0), complete_row(0.0005), complete_row(0.001)]
    )

    data = load_simulation_csv(path)

    assert data.sampling_frequency_hz == pytest.approx(2000.0)
    assert data["platform_displacement_m"].shape == (3,)


def test_rejects_missing_semantic_column(tmp_path: Path) -> None:
    path = tmp_path / "simulation.csv"
    header = tuple(name for name in REQUIRED_COLUMNS if name != "tire_force_n")
    write_csv(path, header, [(0.0,) * len(header), (1.0,) * len(header)])

    with pytest.raises(ValueError, match="tire_force_n"):
        load_simulation_csv(path)


def test_rejects_irregular_sampling(tmp_path: Path) -> None:
    path = tmp_path / "simulation.csv"
    write_csv(path, REQUIRED_COLUMNS, [complete_row(0.0), complete_row(0.1), complete_row(0.25)])

    with pytest.raises(ValueError, match="uniform sampling interval"):
        load_simulation_csv(path)
