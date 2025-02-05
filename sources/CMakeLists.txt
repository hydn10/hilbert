if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-error=unused-function)
endif()

add_subdirectory(hilbert)
add_subdirectory(apps)
