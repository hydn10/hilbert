from pathlib import Path


def plot_egea(file_path: Path, save_path: Path | None, show: bool) -> None:
    from plotter.services.egea_plotting import plot_egea

    plot_egea(file_path, save_path=save_path, show=show)


def validate_egea(file_path: Path) -> str:
    from plotter.services.egea_data import load_egea_data

    data = load_egea_data(file_path)
    return (
        f"raw: {data.raw.data.size} samples at {data.raw.sampling_frequency_hz:g} Hz; "
        f"refined: {data.refined.data.size} samples at "
        f"{data.refined.sampling_frequency_hz:g} Hz"
    )


def plot_phase_scan(file_path: Path, save_path: Path | None, show: bool) -> None:
    from plotter.services.phase_scan_plotting import plot_phase_scan

    plot_phase_scan(file_path, save_path=save_path, show=show)


def validate_phase_scan(file_path: Path) -> str:
    from plotter.services.phase_scan_data import load_phase_scan_data

    data = load_phase_scan_data(file_path)
    frequencies = data["frequency_hz"]
    return f"results: {data.data.size} rows from {frequencies[0]:g} Hz to {frequencies[-1]:g} Hz"
