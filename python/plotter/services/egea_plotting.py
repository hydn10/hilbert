from pathlib import Path

import matplotlib.pyplot as plt

from .egea_data import load_egea_data
from .figures.egea_common import prepare_egea_measurement_plot_data
from .figures.egea_phase_frequency import create_egea_phase_frequency_figure
from .figures.egea_simulation_overview import create_egea_simulation_overview


def _phase_frequency_save_path(save_path: Path) -> Path:
    return save_path.with_name(f"{save_path.stem}-phase-frequency{save_path.suffix}")


def plot_egea(
    file_path: str | Path,
    *,
    save_path: str | Path | None = None,
    show: bool = True,
) -> None:
    simulation = load_egea_data(file_path)
    measurement_plot_data = prepare_egea_measurement_plot_data(simulation)
    overview_figure = create_egea_simulation_overview(measurement_plot_data)
    phase_frequency_figure = create_egea_phase_frequency_figure(measurement_plot_data)
    figures = (overview_figure, phase_frequency_figure)

    try:
        if save_path is not None:
            overview_path = Path(save_path)
            overview_figure.savefig(overview_path)
            phase_frequency_figure.savefig(_phase_frequency_save_path(overview_path))
        if show:
            plt.show()
    finally:
        for figure in figures:
            plt.close(figure)
