cmake_minimum_required(VERSION 4.3)

project(fftw_consumer LANGUAGES CXX)

find_package(FFTW3 CONFIG REQUIRED)

if(NOT TARGET FFTW3::fftw3)
    message(FATAL_ERROR "FFTW3 did not provide FFTW3::fftw3")
endif()

add_executable(fftw-consumer main.cpp)
target_link_libraries(fftw-consumer PRIVATE FFTW3::fftw3)

enable_testing()
add_test(NAME fftw-package-consumer COMMAND fftw-consumer)
