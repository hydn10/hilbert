cmake_minimum_required(VERSION 3.28)

# Reconfigure when the manifest changes, then derive the CMake project identity
# from the native package declaration instead of maintaining a second version.
configure_file(vcpkg.json vcpkg.json COPYONLY)
file(READ "${CMAKE_CURRENT_BINARY_DIR}/vcpkg.json" PROJECT_MANIFEST_JSON)
string(JSON PROJECT_NAME_FROM_JSON GET "${PROJECT_MANIFEST_JSON}" name)
string(JSON PROJECT_VERSION_FROM_JSON GET "${PROJECT_MANIFEST_JSON}" version)

project(
    "${PROJECT_NAME_FROM_JSON}"
    LANGUAGES CXX
    VERSION "${PROJECT_VERSION_FROM_JSON}"
)

# Hilbert does not use C++ modules. Disabling scanning also keeps
# compiler-specific module flags out of clang-tidy commands.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows-runtime.cmake")

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(HILBERT_BUILD_APPS_DEFAULT "${PROJECT_IS_TOP_LEVEL}")
option(
    HILBERT_BUILD_APPS
    "Build the simulation command-line application. Default: ${HILBERT_BUILD_APPS_DEFAULT}."
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

set(project_targets_export "${PROJECT_NAME}-targets")
set(project_package_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

install(
    EXPORT "${project_targets_export}"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${project_package_install_dir}"
    FILE "${project_targets_export}.cmake"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/package-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lib/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "${project_package_install_dir}"
    NO_SET_AND_CHECK_MACRO
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/lib/${PROJECT_NAME}-version.cmake"
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/lib/${PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/lib/${PROJECT_NAME}-version.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindFFTW3.cmake"
    DESTINATION "${project_package_install_dir}"
)
