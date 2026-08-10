if(NOT DEFINED ENV{VCPKG_ROOT} OR "$ENV{VCPKG_ROOT}" STREQUAL "")
    message(FATAL_ERROR "VCPKG_ROOT must point to a vcpkg checkout.")
endif()

set(_HILBERT_VCPKG_TOOLCHAIN
    "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
)

if(NOT EXISTS "${_HILBERT_VCPKG_TOOLCHAIN}")
    message(
        FATAL_ERROR
        "vcpkg toolchain not found: ${_HILBERT_VCPKG_TOOLCHAIN}"
    )
endif()

include("${_HILBERT_VCPKG_TOOLCHAIN}")

unset(_HILBERT_VCPKG_TOOLCHAIN)
