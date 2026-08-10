cmake_minimum_required(VERSION 3.28)

project(hilbert_consumer LANGUAGES CXX)

find_package(hilbert CONFIG REQUIRED)

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
