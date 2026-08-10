set windows-shell := ["powershell.exe", "-NoLogo", "-NoProfile", "-Command"]

import 'just/common.just'

# Source formatting.
mod format 'just/mods/format.just'

# Read-only formatting and linting checks.
mod check 'just/mods/check.just'

# Native CMake and CTest workflows.
mod cpp 'just/mods/cpp.just'

# Python environment, validation, and plotting workflows.
mod py 'just/mods/py.just'

# vcpkg dependency-provider maintenance.
mod vcpkg 'just/mods/vcpkg.just'

# Mise development-environment maintenance.
mod mise 'just/mods/mise.just'

# Disposable build state and Python environments.
mod purge 'just/mods/purge.just'

# List all root and module recipes.
help:
    @just --list --list-submodules

# Run the complete local repository verification.
verify preset=default-quality-preset: check::format::all (check::lint::all preset) (cpp::validate-built preset) (py::validate default-release-preset)
