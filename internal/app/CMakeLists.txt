set(target_name "${PROJECT_NAME}.app")

add_library(${target_name})
add_library("${PROJECT_NAME}::app" ALIAS ${target_name})

target_sources(
    ${target_name}
    PUBLIC
        FILE_SET public_headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/hilbert/app/process/exit_domain.hpp
                include/hilbert/app/process/exit_status.hpp
                include/hilbert/app/process/run.hpp
                include/hilbert/app/cli/argument_cursor.hpp
                include/hilbert/app/cli/error.hpp
                include/hilbert/app/cli/parse.hpp
                include/hilbert/app/cli/run.hpp
                include/hilbert/app/io/output_stream.hpp
                include/hilbert/app/io/numeric_table_writer.hpp
    PRIVATE src/cli/argument_cursor.cpp src/cli/parse.cpp
)

target_compile_features(${target_name} PUBLIC cxx_std_23)
set_target_properties(${target_name} PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)

if(BUILD_SHARED_LIBS)
    set_target_properties(
        ${target_name}
        PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

if(BUILD_SHARED_LIBS AND HILBERT_BUILD_APPS)
    install(
        TARGETS ${target_name}
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
endif()
