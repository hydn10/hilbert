from pathlib import Path


def execute(file_path: Path, save_path: Path | None, show: bool) -> None:
    from plotter.services.plotting import plot_data

    plot_data(file_path, save_path=save_path, show=show)


def validate(file_path: Path) -> str:
    from plotter.services.data import load_simulation_data

    data = load_simulation_data(file_path)
    return (
        f"raw: {data.raw.data.size} samples at {data.raw.sampling_frequency_hz:g} Hz; "
        f"refined: {data.refined.data.size} samples at "
        f"{data.refined.sampling_frequency_hz:g} Hz"
    )
