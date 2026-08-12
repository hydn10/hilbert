set windows-shell := ["powershell.exe", "-NoLogo", "-NoProfile", "-Command"]
set working-directory := "../.."

# Delete all disposable repository-local build and environment state.
all: out python

# Delete the entire out/ directory, including every native build and generated artifact.
out:
    cmake -E remove_directory "out"

# Delete the uv-managed Python environment.
python:
    cmake -E remove_directory "python/.venv"
