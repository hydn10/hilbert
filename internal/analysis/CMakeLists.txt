set(target_name "${PROJECT_NAME}.analysis")

add_library(${target_name} INTERFACE)
add_library("${PROJECT_NAME}::analysis" ALIAS ${target_name})

target_sources(
    ${target_name}
    INTERFACE
        FILE_SET public_headers
            TYPE HEADERS
            BASE_DIRS include
            FILES include/hilbert/analysis/phase.hpp
)

target_compile_features(${target_name} INTERFACE cxx_std_23)
target_link_libraries(${target_name} INTERFACE "${PROJECT_NAME}::${PROJECT_NAME}")
