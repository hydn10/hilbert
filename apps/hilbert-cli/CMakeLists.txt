set(target_name "${PROJECT_NAME}-cli")

add_executable(${target_name})
target_sources(
    ${target_name}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/collectors.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/derivative.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/rk4.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/state.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/collectors.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)

target_include_directories(
    ${target_name}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_features(${target_name} PRIVATE cxx_std_23)
target_link_libraries(${target_name} PRIVATE "${PROJECT_NAME}::${PROJECT_NAME}")

stage_runtime_dlls(${target_name})

install(TARGETS ${target_name})
