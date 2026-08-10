set(expected_header
    "time_s,sprung_displacement_m,unsprung_displacement_m,platform_displacement_m,tire_force_n,platform_amplitude_m,platform_phase_rad,platform_frequency_hz,tire_force_amplitude_n,tire_force_phase_rad,tire_force_frequency_hz"
)

execute_process(
    COMMAND "${CLI}" --duration 0.01 --output "${OUTPUT}"
    RESULT_VARIABLE simulation_result
    ERROR_VARIABLE simulation_error
)

if(NOT simulation_result EQUAL 0)
    message(
        FATAL_ERROR
        "hilbert-cli failed (${simulation_result}): ${simulation_error}"
    )
endif()

file(STRINGS "${OUTPUT}" actual_header LIMIT_COUNT 1)
if(NOT actual_header STREQUAL expected_header)
    message(FATAL_ERROR "simulation CSV header changed:\n${actual_header}")
endif()

file(STRINGS "${OUTPUT}" output_lines)
list(LENGTH output_lines output_line_count)
if(NOT output_line_count EQUAL 22)
    message(
        FATAL_ERROR
        "expected one header and 21 samples, got ${output_line_count} lines"
    )
endif()
