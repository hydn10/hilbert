from pathlib import Path


def execute(file_path: Path, save_path: Path | None, show: bool) -> None:
    from plotter.services.plotting import plot_data

    plot_data(file_path, save_path=save_path, show=show)


def validate(file_path: Path) -> str:
    from plotter.services.data import load_simulation_csv

    data = load_simulation_csv(file_path)
    sample_count = data["time_s"].size
    return f"{sample_count} samples at {data.sampling_frequency_hz:g} Hz"
