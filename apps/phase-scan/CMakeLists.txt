set(target_name "${PROJECT_NAME}-phase-scan")

add_executable(${target_name})
target_sources(
    ${target_name}
    PRIVATE src/phase-scan.cpp src/main.cpp
    PRIVATE
        FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/hilbert/phase_scan/estimators/hilbert.hpp
                include/hilbert/phase_scan/estimators/least_squares.hpp
                include/hilbert/phase_scan/output.hpp
                include/hilbert/phase_scan/phase_scan.hpp
                include/hilbert/phase_scan/result.hpp
)

target_compile_features(${target_name} PRIVATE cxx_std_23)
target_link_libraries(
    ${target_name}
    PRIVATE "${PROJECT_NAME}::${PROJECT_NAME}" "${PROJECT_NAME}::app"
)
set_target_properties(${target_name} PROPERTIES VERIFY_PRIVATE_HEADER_SETS ON)

stage_runtime_dlls(${target_name})

install(TARGETS ${target_name})
