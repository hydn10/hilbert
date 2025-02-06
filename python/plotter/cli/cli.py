import typer

from .commands import cli_cmds


app = typer.Typer()


@app.command()
def execute(file_path: str):
    cli_cmds.execute(file_path)