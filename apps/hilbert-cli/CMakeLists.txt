set(target_name "${PROJECT_NAME}-cli")

add_executable(${target_name})
target_sources(
    ${target_name}
    PRIVATE src/cli.cpp src/main.cpp
    PRIVATE
        FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/hilbertcli/cli.hpp
                include/hilbertcli/output.hpp
                include/hilbertcli/process/exit_domain.hpp
                include/hilbertcli/process/exit_status.hpp
                include/hilbertcli/simulation.hpp
)

target_compile_features(${target_name} PRIVATE cxx_std_23)
target_link_libraries(${target_name} PRIVATE "${PROJECT_NAME}::${PROJECT_NAME}")
set_target_properties(${target_name} PROPERTIES VERIFY_PRIVATE_HEADER_SETS ON)

stage_runtime_dlls(${target_name})

install(TARGETS ${target_name})
