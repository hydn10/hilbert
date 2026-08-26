find_package(FFTW3 CONFIG REQUIRED)

set(target_name "${PROJECT_NAME}")

add_library(${target_name})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS ${target_name})

target_sources(
    ${target_name}
    PUBLIC
        FILE_SET public_headers
            TYPE HEADERS
            BASE_DIRS include/public
            FILES
                include/public/hilbert/analysis.hpp
                include/public/hilbert/analysis/least_squares/basis.hpp
                include/public/hilbert/analysis/least_squares/basis_value.hpp
                include/public/hilbert/analysis/least_squares/detail/discrete_reduction.hpp
                include/public/hilbert/analysis/least_squares/least_squares_diagnostics.hpp
                include/public/hilbert/analysis/least_squares/normal_equations3.hpp
                include/public/hilbert/analysis/least_squares/observation.hpp
                include/public/hilbert/analysis/least_squares/observation_count.hpp
                include/public/hilbert/analysis/least_squares/least_squares_products.hpp
                include/public/hilbert/analysis/least_squares/least_squares_residual_statistics.hpp
                include/public/hilbert/analysis/phase/circular_mean.hpp
                include/public/hilbert/analysis/phase/circular_mean_result.hpp
                include/public/hilbert/analysis/phase/mean_resultant_length.hpp
                include/public/hilbert/analysis/phase/principal_phase.hpp
                include/public/hilbert/analysis/frequency_response.hpp
                include/public/hilbert/analysis/sampling/sample_window.hpp
                include/public/hilbert/analysis/signals/hilbert_transform.hpp
                include/public/hilbert/analysis/signals/signal_data.hpp
                include/public/hilbert/analysis/signals/remove_dc_component.hpp
                include/public/hilbert/analysis/sinusoidal/basis.hpp
                include/public/hilbert/analysis/sinusoidal/sinusoidal_fit.hpp
                include/public/hilbert/analysis/sinusoidal/frequency_hz.hpp
                include/public/hilbert/core/supported_float.hpp
                include/public/hilbert/detail/attributes.hpp
                include/public/hilbert/hilbert.hpp
                include/public/hilbert/math.hpp
                include/public/hilbert/math/linear_algebra/cholesky_factor3.hpp
                include/public/hilbert/math/linear_algebra/signature.hpp
                include/public/hilbert/math/linear_algebra/symmetric_eigenvalues3.hpp
                include/public/hilbert/math/linear_algebra/symmetric_matrix.hpp
                include/public/hilbert/math/linear_algebra/vector.hpp
                include/public/hilbert/simulation.hpp
                include/public/hilbert/simulation/generic.hpp
                include/public/hilbert/simulation/core/concepts.hpp
                include/public/hilbert/simulation/core/input_count.hpp
                include/public/hilbert/simulation/core/problem.hpp
                include/public/hilbert/simulation/core/settings.hpp
                include/public/hilbert/simulation/detail/simulation_engine.hpp
                include/public/hilbert/simulation/drivers/run.hpp
                include/public/hilbert/simulation/drivers/simulation_view.hpp
                include/public/hilbert/simulation/integrators/rk4.hpp
                include/public/hilbert/simulation/sinks/adapters.hpp
                include/public/hilbert/simulation/sinks/concepts.hpp
                include/public/hilbert/simulation/sinks/normal_equations.hpp
                include/public/hilbert/simulation/suspension.hpp
                include/public/hilbert/simulation/suspension/factory.hpp
                include/public/hilbert/simulation/suspension/frequency.hpp
                include/public/hilbert/simulation/suspension/ground_frequencies/constant.hpp
                include/public/hilbert/simulation/suspension/ground_frequencies/scheduled.hpp
                include/public/hilbert/simulation/suspension/model.hpp
                include/public/hilbert/simulation/suspension/parameters.hpp
                include/public/hilbert/simulation/suspension/sample.hpp
                include/public/hilbert/simulation/suspension/sinks/soa_vector.hpp
                include/public/hilbert/simulation/suspension/sinks/vector.hpp
                include/public/hilbert/simulation/suspension/state.hpp
    PRIVATE
        FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS include/private
            FILES include/private/hilbert/detail/fft.hpp
    PRIVATE
        src/analysis/hilbert_transform.cpp
        src/analysis/signal_data.cpp
        src/detail/fft.cpp
)

target_compile_features(${target_name} PUBLIC cxx_std_23)
target_link_libraries(${target_name} PRIVATE FFTW3::fftw3)

set_target_properties(
    ${target_name}
    PROPERTIES
        EXPORT_NAME "${PROJECT_NAME}"
        VERIFY_INTERFACE_HEADER_SETS ON
        VERIFY_PRIVATE_HEADER_SETS ON
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
