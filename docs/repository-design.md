# Repository design

Hilbert has two intentionally separate applications around one file boundary:

1. A reusable C++ Hilbert-transform library and a native suspension simulator built by CMake.
2. A standard Python plotting application that consumes the simulator's named-column CSV output.

There is no native Python extension. The CSV boundary is sufficient for the project's batch simulation and analysis workflow and keeps the native and Python projects independently usable.

The repository follows an inside-out ownership model:

- `vcpkg.json` owns the project name, version, and semantic native dependencies.
- CMake owns native targets, public/private interfaces, tests, installation, and package export.
- `python/pyproject.toml` and `python/uv.lock` own the plotter package and Python dependency graph.
- vcpkg and Nix provision dependencies without redefining native targets.
- Mise provisions portable repository tools, not the compiler, linker, SDK, or native platform setup.
- Just delegates discoverable workflows to CMake, CTest, uv, and the formatters. It is not a build system.

Generic CMake presets remain provider-independent. Provider-specific presets compose the vcpkg adapter with those same configurations. A caller-supplied `CMAKE_TOOLCHAIN_FILE` takes priority in the Just defaults, so external toolchains remain ordinary CMake inputs.

Nix is the holistic outer environment. Its native dependency mapping derives from `vcpkg.json`, its Python application derives from uv metadata, and its development tools derive from Mise metadata. The core CMake and Python workflows do not require Nix, Mise, Just, or vcpkg.
