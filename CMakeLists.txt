cmake_minimum_required(VERSION 4.3)

# Reconfigure when the manifest changes, then derive the CMake project identity
# from the native package declaration instead of maintaining a second version.
configure_file(vcpkg.json vcpkg.json COPYONLY)
file(READ "${CMAKE_CURRENT_BINARY_DIR}/vcpkg.json" PROJECT_MANIFEST_JSON)
string(JSON PROJECT_NAME_FROM_JSON GET "${PROJECT_MANIFEST_JSON}" name)
string(JSON PROJECT_VERSION_FROM_JSON GET "${PROJECT_MANIFEST_JSON}" version)

project(
    "${PROJECT_NAME_FROM_JSON}"
    VERSION "${PROJECT_VERSION_FROM_JSON}"
    SPDX_LICENSE
    "MIT"
    HOMEPAGE_URL "https://github.com/hydn10/hilbert"
    LANGUAGES CXX
)

# Hilbert does not use C++ modules. Disabling scanning also keeps
# compiler-specific module flags out of clang-tidy commands.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(GNUInstallDirs)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows-runtime.cmake")

set(HILBERT_BUILD_APPS_DEFAULT "${PROJECT_IS_TOP_LEVEL}")
option(
    HILBERT_BUILD_APPS
    "Build the simulation experiment applications. Default: ${HILBERT_BUILD_APPS_DEFAULT}."
    "${HILBERT_BUILD_APPS_DEFAULT}"
)

set(HILBERT_BUILD_TESTING_DEFAULT "${PROJECT_IS_TOP_LEVEL}")
option(
    HILBERT_BUILD_TESTING
    "Build the native tests. Default: ${HILBERT_BUILD_TESTING_DEFAULT}."
    "${HILBERT_BUILD_TESTING_DEFAULT}"
)

add_subdirectory(lib)

if(HILBERT_BUILD_APPS)
    add_subdirectory(apps)
endif()

if(HILBERT_BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

install(
    PACKAGE_INFO "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}-targets"
    VERSION "${PROJECT_VERSION}"
    DEFAULT_TARGETS "${PROJECT_NAME}"
    LOWER_CASE_FILE
)
