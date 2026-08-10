set(target_name "${PROJECT_NAME}.tests.unit")

add_executable(${target_name})
target_sources(
    ${target_name}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test_hilbert.cpp"
)
target_compile_features(${target_name} PRIVATE cxx_std_23)
target_link_libraries(${target_name} PRIVATE "${PROJECT_NAME}::${PROJECT_NAME}")

stage_runtime_dlls(${target_name})

add_test(NAME hilbert-numerics COMMAND ${target_name})

if(HILBERT_BUILD_APPS)
    add_test(
        NAME simulation-data-contract
        COMMAND
            "${CMAKE_COMMAND}" "-DCLI=$<TARGET_FILE:${PROJECT_NAME}-cli>"
            "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/simulation-contract.csv" -P
            "${CMAKE_CURRENT_SOURCE_DIR}/check_simulation_contract.cmake"
    )
endif()
