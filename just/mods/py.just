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

# Generate representative EGEA and constant-frequency-sweep data, validate it, and render both workflows headlessly.
smoke preset=default-release-preset output_dir=default-output-dir: (prepare-output output_dir)
    just cpp simulate-egea "{{ output_dir }}/simulation-egea.txt" {{ preset }} 20
    uv run --project python --locked plotter egea validate "{{ output_dir }}/simulation-egea.txt"
    uv run --project python --locked plotter egea plot "{{ output_dir }}/simulation-egea.txt" --save "{{ output_dir }}/simulation-egea.png" --no-show
    just cpp simulate-frequency-sweep "{{ output_dir }}/simulation-frequency-sweep.txt" {{ preset }} 1 5 1
    uv run --project python --locked plotter frequency-sweep validate "{{ output_dir }}/simulation-frequency-sweep.txt"
    uv run --project python --locked plotter frequency-sweep plot "{{ output_dir }}/simulation-frequency-sweep.txt" --save "{{ output_dir }}/simulation-frequency-sweep.png" --no-show

# Validate EGEA simulation data without opening a plot.
validate-egea input:
    uv run --project python --locked plotter egea validate "{{ input }}"

# Run the EGEA plotting CLI (usage: just py plot-egea data.txt --save plot.png).
plot-egea input *args:
    uv run --project python --locked plotter egea plot "{{ input }}" {{ args }}

# Validate a constant-frequency sweep result file.
validate-frequency-sweep input:
    uv run --project python --locked plotter frequency-sweep validate "{{ input }}"

# Plot a constant-frequency sweep result file.
plot-frequency-sweep input *args:
    uv run --project python --locked plotter frequency-sweep plot "{{ input }}" {{ args }}
