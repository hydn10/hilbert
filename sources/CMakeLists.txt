if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

find_package(Eigen3 REQUIRED)
find_package(FFTW3 REQUIRED)

add_subdirectory(hilbert)
add_subdirectory(apps)
