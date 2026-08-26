from pathlib import Path
from typing import Annotated

import typer

from .commands import cli_cmds

app = typer.Typer(no_args_is_help=True)
egea_app = typer.Typer(
    no_args_is_help=True, help="Plot and validate EGEA scheduled-frequency output."
)
phase_scan_app = typer.Typer(
    no_args_is_help=True,
    help="Plot and validate phase-scan frequency-response output.",
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


@phase_scan_app.command("plot")
def phase_scan_plot(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
    save: Annotated[
        Path | None,
        typer.Option(
            "--save",
            help="Save the response figure to this path and diagnostics to a '-diagnostics' sibling path.",
        ),
    ] = None,
    show: Annotated[
        bool,
        typer.Option("--show/--no-show", help="Open the interactive plot window."),
    ] = True,
) -> None:
    """Plot response estimates and diagnostics from a constant-frequency phase scan."""
    cli_cmds.plot_phase_scan(file_path, save, show)


@phase_scan_app.command("validate")
def phase_scan_validate(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
) -> None:
    """Validate the phase-scan response and diagnostics contract without opening a plot."""
    typer.echo(cli_cmds.validate_phase_scan(file_path))


app.add_typer(egea_app, name="egea")
app.add_typer(phase_scan_app, name="phase-scan")
