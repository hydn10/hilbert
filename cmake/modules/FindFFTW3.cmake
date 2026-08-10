# Normalize the FFTW package interfaces supplied by vcpkg, Nixpkgs, and common
# system installations into the single semantic target used by Hilbert.

find_package(FFTW3 CONFIG QUIET)

if(TARGET FFTW3::fftw3)
    set(FFTW3_FOUND TRUE)
    return()
endif()

if(TARGET fftw3)
    add_library(FFTW3::fftw3 INTERFACE IMPORTED)
    target_link_libraries(FFTW3::fftw3 INTERFACE fftw3)
    set(FFTW3_FOUND TRUE)
    return()
endif()

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_FFTW3 QUIET fftw3)
endif()

find_path(FFTW3_INCLUDE_DIR NAMES fftw3.h HINTS ${PC_FFTW3_INCLUDE_DIRS})
find_library(FFTW3_LIBRARY NAMES fftw3 HINTS ${PC_FFTW3_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FFTW3
    REQUIRED_VARS FFTW3_LIBRARY FFTW3_INCLUDE_DIR
)

if(FFTW3_FOUND AND NOT TARGET FFTW3::fftw3)
    add_library(FFTW3::fftw3 UNKNOWN IMPORTED)
    set_target_properties(
        FFTW3::fftw3
        PROPERTIES
            IMPORTED_LOCATION "${FFTW3_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${FFTW3_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(FFTW3_INCLUDE_DIR FFTW3_LIBRARY)
