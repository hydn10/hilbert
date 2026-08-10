set windows-shell := ["powershell.exe", "-NoLogo", "-NoProfile", "-Command"]
set working-directory := "../../.."

import '../../common.just'

# Run all available linters.
[default]
all preset=default-quality-preset: (cpp preset) py ci

# Run clang-tidy across every native target.
cpp preset=default-quality-preset:
    cmake --preset {{ preset }} --fresh
    cmake --build --preset {{ preset }} --clean-first

# Run the Ruff linter across all Python files.
py:
    uv run --project python --locked --only-group dev ruff check python

# Validate GitHub Actions workflows.
ci:
    actionlint
