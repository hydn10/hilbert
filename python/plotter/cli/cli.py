from pathlib import Path
from typing import Annotated

import typer

from .commands import cli_cmds

app = typer.Typer(no_args_is_help=True)
egea_app = typer.Typer(
    no_args_is_help=True, help="Plot and validate EGEA scheduled-frequency output."
)
frequency_sweep_app = typer.Typer(
    no_args_is_help=True,
    help="Plot and validate constant-frequency sweep output.",
)


@egea_app.command("plot")
def egea_plot(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
    save: Annotated[
        Path | None,
        typer.Option(
            "--save",
            help="Save both figures. The phase-frequency filename gets a '-phase-frequency' suffix.",
        ),
    ] = None,
    show: Annotated[
        bool,
        typer.Option("--show/--no-show", help="Open the interactive plot windows."),
    ] = True,
) -> None:
    """Plot one EGEA scheduled-frequency simulation in two figures."""
    cli_cmds.plot_egea(file_path, save, show)


@egea_app.command("validate")
def egea_validate(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
) -> None:
    """Validate the EGEA multi-table text contract without opening a plot."""
    typer.echo(cli_cmds.validate_egea(file_path))


@frequency_sweep_app.command("plot")
def frequency_sweep_plot(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
    save: Annotated[
        Path | None,
        typer.Option("--save", help="Save the phase-versus-frequency figure to this path."),
    ] = None,
    show: Annotated[
        bool,
        typer.Option("--show/--no-show", help="Open the interactive plot window."),
    ] = True,
) -> None:
    """Plot both phase estimates from a constant-frequency sweep."""
    cli_cmds.plot_frequency_sweep(file_path, save, show)


@frequency_sweep_app.command("validate")
def frequency_sweep_validate(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
) -> None:
    """Validate the constant-frequency sweep results contract without opening a plot."""
    typer.echo(cli_cmds.validate_frequency_sweep(file_path))


app.add_typer(egea_app, name="egea")
app.add_typer(frequency_sweep_app, name="frequency-sweep")
