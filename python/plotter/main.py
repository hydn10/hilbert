from .cli.cli import app


APP_NAME = "plotter"


def run_cli():
    app(prog_name=APP_NAME)