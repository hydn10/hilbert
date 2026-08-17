set(target_name "${PROJECT_NAME}.app")

add_library(${target_name} INTERFACE)
add_library("${PROJECT_NAME}::app" ALIAS ${target_name})

target_sources(
    ${target_name}
    INTERFACE
        FILE_SET public_headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/hilbert/app/process/exit_domain.hpp
                include/hilbert/app/process/exit_status.hpp
)

target_compile_features(${target_name} INTERFACE cxx_std_23)
set_target_properties(${target_name} PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)
