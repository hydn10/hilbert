import typer

from .commands import cli_cmds


app = typer.Typer()


@app.command()
def execute():
    cli_cmds.execute()