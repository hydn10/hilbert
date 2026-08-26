from pathlib import Path

import matplotlib.pyplot as plt

from .figures.phase_scan_diagnostics import create_phase_scan_diagnostics_figure
from .figures.phase_scan_frequency_response import (
    create_phase_scan_frequency_response_figure,
)
from .phase_scan_data import load_phase_scan_data


def _diagnostics_save_path(save_path: Path) -> Path:
    return save_path.with_name(f"{save_path.stem}-diagnostics{save_path.suffix}")


def plot_phase_scan(
    file_path: str | Path,
    *,
    save_path: str | Path | None = None,
    show: bool = True,
) -> None:
    results = load_phase_scan_data(file_path)
    response_figure = create_phase_scan_frequency_response_figure(results)
    diagnostics_figure = create_phase_scan_diagnostics_figure(results)
    figures = (response_figure, diagnostics_figure)

    try:
        if save_path is not None:
            response_path = Path(save_path)
            response_figure.savefig(response_path)
            diagnostics_figure.savefig(_diagnostics_save_path(response_path))
        if show:
            plt.show()
    finally:
        for figure in figures:
            plt.close(figure)
