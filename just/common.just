external-toolchain := env_var_or_default("CMAKE_TOOLCHAIN_FILE", "")
vcpkg-root := env_var_or_default("VCPKG_ROOT", "")
provider-prefix := if external-toolchain != "" { "" } else if vcpkg-root != "" { "vcpkg-" } else { "" }

default-quality-preset := provider-prefix + "quality"
default-release-preset := provider-prefix + "release"
