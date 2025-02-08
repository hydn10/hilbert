cmake_minimum_required(VERSION 3.21)

file(READ vcpkg.json VCPKG_JSON_STRING)
string(JSON PROJ_NAME_FROM_JSON GET ${VCPKG_JSON_STRING} name)
string(JSON VERSION_FROM_JSON GET ${VCPKG_JSON_STRING} version)

project(${PROJ_NAME_FROM_JSON} LANGUAGES CXX VERSION ${VERSION_FROM_JSON})

find_package(FFTW3 REQUIRED)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-error=unused-function)
endif()

add_subdirectory(hilbert)
add_subdirectory(apps)

