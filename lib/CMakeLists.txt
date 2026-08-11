find_package(FFTW3 CONFIG REQUIRED)

set(target_name "${PROJECT_NAME}")

add_library(${target_name})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS ${target_name})

target_sources(
    ${target_name}
    PUBLIC
        FILE_SET public_headers
            TYPE HEADERS
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/public"
            FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/include/public/hilbert/hilbert.hpp"
    PRIVATE
        FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/private"
            FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/include/private/hilbert/detail/fft.hpp"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fft.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/hilbert.cpp"
)

target_compile_features(${target_name} PUBLIC cxx_std_23)
target_link_libraries(${target_name} PRIVATE FFTW3::fftw3)

set_target_properties(
    ${target_name}
    PROPERTIES EXPORT_NAME "${PROJECT_NAME}" VERIFY_INTERFACE_HEADER_SETS ON
)

if(BUILD_SHARED_LIBS)
    set_target_properties(
        ${target_name}
        PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

install(
    TARGETS ${target_name}
    EXPORT "${PROJECT_NAME}-targets"
    FILE_SET public_headers
)
