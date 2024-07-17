set(target hilbert)

add_library(${target} "src/hilbert.cpp")

target_include_directories(${target} PUBLIC include)
target_compile_features (${target} PUBLIC cxx_std_23)

target_link_libraries(${target} FFTW3::fftw3)