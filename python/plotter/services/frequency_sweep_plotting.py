from pathlib import Path

import matplotlib.pyplot as plt

from .figures.frequency_sweep_phase_frequency import create_frequency_sweep_phase_frequency_figure
from .frequency_sweep_data import load_frequency_sweep_data


def plot_frequency_sweep(
    file_path: str | Path,
    *,
    save_path: str | Path | None = None,
    show: bool = True,
) -> None:
    results = load_frequency_sweep_data(file_path)
    figure = create_frequency_sweep_phase_frequency_figure(results)

    try:
        if save_path is not None:
            figure.savefig(Path(save_path))
        if show:
            plt.show()
    finally:
        plt.close(figure)
