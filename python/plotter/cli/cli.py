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
        typer.Option("--save", help="Save the plot to this path."),
    ] = None,
    show: Annotated[
        bool,
        typer.Option("--show/--no-show", help="Open an interactive plot window."),
    ] = True,
) -> None:
    """Plot one Hilbert simulation CSV file."""
    cli_cmds.execute(file_path, save, show)


@app.command()
def validate(
    file_path: Annotated[Path, typer.Argument(exists=True, dir_okay=False, readable=True)],
) -> None:
    """Validate the CSV contract without opening a plot."""
    typer.echo(cli_cmds.validate(file_path))
