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


def plot_frequency_sweep(file_path: Path, save_path: Path | None, show: bool) -> None:
    from plotter.services.frequency_sweep_plotting import plot_frequency_sweep

    plot_frequency_sweep(file_path, save_path=save_path, show=show)


def validate_frequency_sweep(file_path: Path) -> str:
    from plotter.services.frequency_sweep_data import load_frequency_sweep_data

    data = load_frequency_sweep_data(file_path)
    frequencies = data["frequency_hz"]
    return f"results: {data.data.size} rows from {frequencies[0]:g} Hz to {frequencies[-1]:g} Hz"
