set windows-shell := ["powershell.exe", "-NoLogo", "-NoProfile", "-Command"]
set working-directory := "../.."

import '../common.just'

repo-root := justfile_directory()
executable-suffix := if os() == "windows" { ".exe" } else { "" }

# List all available CMake presets.
list:
    cmake --list-presets=all

# Configure the native build.
[arg("fresh", long="fresh", value="true")]
configure preset=default-quality-preset fresh="false":
    cmake --preset {{ preset }} {{ if fresh == "true" { "--fresh" } else { "" } }}

# Build the configured native tree.
build preset=default-quality-preset:
    cmake --build --preset {{ preset }}

# Clean built artifacts while preserving the configured native tree.
clean preset=default-quality-preset:
    cmake --build --preset {{ preset }} --target clean

# Run tests from the configured native tree.
test preset=default-quality-preset:
    ctest --preset {{ preset }}

# Verify that every public header can be included on its own.
verify-headers preset=default-quality-preset:
    cmake --build --preset {{ preset }} --target all_verify_header_sets

[private]
fresh-build preset: (configure preset "true")
    cmake --build --preset {{ preset }} --clean-first

[private]
validate-built preset: (verify-headers preset) (test preset)

# Freshly configure, cleanly build, validate public headers, and test the native project.
validate preset=default-quality-preset: (fresh-build preset) (validate-built preset)

# Freshly build and validate a native package, then test it from an installed consumer.
[arg("preset", pattern="quality|release|shared-release|vcpkg-quality|vcpkg-release|vcpkg-shared-release")]
check-installed preset=default-release-preset: (fresh-build preset) (validate-built preset)
    cmake --install "out/build/{{ preset }}"
    cmake -S tests/consumer-cmake -B "out/build/consumer-cmake/{{ preset }}" --fresh -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH={{ repo-root }}/out/install/{{ preset }}" "-DHILBERT_EXPECT_FFTW_TARGET={{ if preset =~ "shared-release$" { "OFF" } else { "ON" } }}" {{ if preset =~ "^vcpkg-" { "-DCMAKE_TOOLCHAIN_FILE=" + repo-root + "/cmake/toolchains/vcpkg.cmake" } else { "" } }} {{ if preset =~ "^vcpkg-" { "-DVCPKG_INSTALLED_DIR=" + repo-root + "/out/build/" + preset + "/vcpkg_installed" } else { "" } }}
    cmake --build "out/build/consumer-cmake/{{ preset }}" --clean-first
    cmake --build "out/build/consumer-cmake/{{ preset }}" --target setup-installed-consumer-runtime-dependencies
    ctest --test-dir "out/build/consumer-cmake/{{ preset }}" --output-on-failure --no-tests=error

# Generate multi-table simulation data with the selected native build.
simulate-egea output="out/simulation-egea.txt" preset=default-release-preset duration="20":
    cmake --preset {{ preset }}
    cmake --build --preset {{ preset }} --target hilbert-egea
    cmake -E make_directory "{{ parent_directory(output) }}"
    out/build/{{ preset }}/apps/hilbert-egea/hilbert-egea{{ executable-suffix }} --duration {{ duration }} --output "{{ output }}"

# Generate constant-frequency sweep results with the selected native build.
simulate-frequency-sweep output="out/simulation-frequency-sweep.txt" preset=default-release-preset start_frequency="1" end_frequency="25" frequency_step="1":
    cmake --preset {{ preset }}
    cmake --build --preset {{ preset }} --target hilbert-frequency-sweep
    cmake -E make_directory "{{ parent_directory(output) }}"
    out/build/{{ preset }}/apps/hilbert-frequency-sweep/hilbert-frequency-sweep{{ executable-suffix }} --start-frequency {{ start_frequency }} --end-frequency {{ end_frequency }} --frequency-step {{ frequency_step }} --output "{{ output }}"
