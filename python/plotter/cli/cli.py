from pathlib import Path
from typing import Annotated

import typer

from .commands import cli_cmds

app = typer.Typer(no_args_is_help=True)


@app.command()
def execute(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
    save: Annotated[
        Path | None,
        typer.Option(
            "--save",
            help="Save both figures; the phase-frequency filename gets a '-phase-frequency' suffix.",
        ),
    ] = None,
    show: Annotated[
        bool,
        typer.Option("--show/--no-show", help="Open the interactive plot windows."),
    ] = True,
) -> None:
    """Plot one Hilbert simulation data file in two figures."""
    cli_cmds.execute(file_path, save, show)


@app.command()
def validate(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
) -> None:
    """Validate the multi-table text contract without opening a plot."""
    typer.echo(cli_cmds.validate(file_path))
