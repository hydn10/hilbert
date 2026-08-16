set(target_name "${PROJECT_NAME}-egea")

add_executable(${target_name})
target_sources(
    ${target_name}
    PRIVATE src/egea.cpp src/main.cpp
    PRIVATE
        FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/hilbert_egea/egea.hpp
                include/hilbert_egea/output.hpp
                include/hilbert_egea/process/exit_domain.hpp
                include/hilbert_egea/process/exit_status.hpp
                include/hilbert_egea/simulation.hpp
)

target_compile_features(${target_name} PRIVATE cxx_std_23)
target_link_libraries(${target_name} PRIVATE "${PROJECT_NAME}::${PROJECT_NAME}")
set_target_properties(${target_name} PROPERTIES VERIFY_PRIVATE_HEADER_SETS ON)

stage_runtime_dlls(${target_name})

install(TARGETS ${target_name})
