set(target hilbert)

add_library(${target} "src/hilbert.cpp" "src/fft.cpp" )

target_include_directories(${target} PUBLIC include)
target_compile_features (${target} PUBLIC cxx_std_23)

# Vcpkg and Nixpkgs handle namespaces differently.
if (TARGET FFTW3::fftw3)
    target_link_libraries(${target} PUBLIC FFTW3::fftw3)
else()
    target_link_libraries(${target} PUBLIC fftw3)
endif()
