set windows-shell := ["powershell.exe", "-NoLogo", "-NoProfile", "-Command"]
set working-directory := "../.."

import '../common.just'

default-output-dir := "out/smoke/python"

[private]
prepare-output output_dir:
    cmake -E make_directory "{{ output_dir }}"

# Create or synchronize the locked Python environment.
sync:
    uv sync --project python --locked

# Update the lockfile to the newest dependency versions allowed by the project.
upgrade:
    uv lock --project python --upgrade

# Validate the Python application and its native multi-table text integration.
validate preset=default-release-preset: (smoke preset default-output-dir)

# Generate representative native data, validate it, and render both figures headlessly.
smoke preset=default-release-preset output_dir=default-output-dir: (prepare-output output_dir)
    just cpp simulate "{{ output_dir }}/simulation.txt" {{ preset }} 20
    uv run --project python --locked plotter validate "{{ output_dir }}/simulation.txt"
    uv run --project python --locked plotter execute "{{ output_dir }}/simulation.txt" --save "{{ output_dir }}/simulation.png" --no-show

# Validate a simulation data file without opening a plot.
validate-data input:
    uv run --project python --locked plotter validate "{{ input }}"

# Run the plotting CLI (usage: just py plot data.txt --save plot.png).
plot input *args:
    uv run --project python --locked plotter execute "{{ input }}" {{ args }}
