from pathlib import Path

import matplotlib.pyplot as plt

from .figures.phase_scan_phase_frequency import create_phase_scan_phase_frequency_figure
from .phase_scan_data import load_phase_scan_data


def plot_phase_scan(
    file_path: str | Path,
    *,
    save_path: str | Path | None = None,
    show: bool = True,
) -> None:
    results = load_phase_scan_data(file_path)
    figure = create_phase_scan_phase_frequency_figure(results)

    try:
        if save_path is not None:
            figure.savefig(Path(save_path))
        if show:
            plt.show()
    finally:
        plt.close(figure)
