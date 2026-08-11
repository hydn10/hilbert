cmake_minimum_required(VERSION 4.3)

project(hilbert_consumer LANGUAGES CXX)

if(NOT DEFINED HILBERT_EXPECT_FFTW_TARGET)
    message(FATAL_ERROR "HILBERT_EXPECT_FFTW_TARGET must be ON or OFF")
endif()

if(NOT DEFINED HILBERT_PACKAGE_VERSION)
    file(READ "${CMAKE_CURRENT_LIST_DIR}/../../vcpkg.json" manifest_json)
    string(JSON HILBERT_PACKAGE_VERSION GET "${manifest_json}" version)
endif()

# Exact-version compatibility must reject even an older 0.x request that
# SameMajorVersion would accept.
find_package(hilbert 0.0.1 CONFIG QUIET)
if(hilbert_FOUND)
    message(
        FATAL_ERROR
        "hilbert ${HILBERT_PACKAGE_VERSION} incorrectly accepted a 0.0.1 request"
    )
endif()
unset(hilbert_DIR CACHE)

find_package(hilbert ${HILBERT_PACKAGE_VERSION} EXACT CONFIG REQUIRED)

if(NOT hilbert_CONFIG MATCHES "[.]cps$")
    message(FATAL_ERROR "Hilbert was not discovered through its CPS package")
endif()

if(HILBERT_EXPECT_FFTW_TARGET AND NOT TARGET FFTW3::fftw3)
    message(FATAL_ERROR "the static package did not resolve FFTW3::fftw3")
elseif(NOT HILBERT_EXPECT_FFTW_TARGET AND TARGET FFTW3::fftw3)
    message(
        FATAL_ERROR
        "the shared package unnecessarily resolved FFTW3::fftw3"
    )
endif()

add_executable(hilbert-consumer main.cpp)
target_compile_features(hilbert-consumer PRIVATE cxx_std_23)
target_link_libraries(hilbert-consumer PRIVATE hilbert::hilbert)

if(WIN32)
    add_custom_command(
        TARGET hilbert-consumer
        POST_BUILD
        COMMAND
            "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:hilbert-consumer>"
            "$<TARGET_RUNTIME_DLLS:hilbert-consumer>"
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endif()

enable_testing()
add_test(NAME installed-package-consumer COMMAND hilbert-consumer)
