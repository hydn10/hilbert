#include <hilbert/hilbert.hpp>

#include <cmath>
#include <complex>
#include <concepts>
#include <exception>
#include <iostream>
#include <limits>
#include <numbers>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <vector>


namespace
{

using reordered_sinusoidal_signature = hilbert::math::
    signature<hilbert::analysis::sine_term, hilbert::analysis::cosine_term, hilbert::analysis::constant_term>;
using sinusoidal_basis = decltype(hilbert::analysis::make_sinusoidal_basis(hilbert::analysis::frequency_hz<double>{1}));
using dual_sinusoidal_signature = hilbert::math::dual<hilbert::analysis::sinusoidal_signature>;

static_assert(hilbert::math::signature_for_size<hilbert::analysis::sinusoidal_signature, 3uz>);
static_assert(!hilbert::math::signature_for_size<dual_sinusoidal_signature, 3uz>);
static_assert(hilbert::math::coordinate_signature_for_size<dual_sinusoidal_signature, 3uz>);
static_assert(hilbert::math::signature_contains<
              hilbert::analysis::sinusoidal_signature const &,
              hilbert::analysis::cosine_term>);
static_assert(!hilbert::analysis::sinusoidal_coordinate_signature<dual_sinusoidal_signature>);
static_assert(hilbert::analysis::basis_for_size<sinusoidal_basis, 3uz, double>);
static_assert(
    hilbert::analysis::least_squares_observation_for<hilbert::analysis::least_squares_observation<double, 2>, double>);
static_assert(
    !hilbert::analysis::least_squares_observation_for<hilbert::analysis::least_squares_observation<double, 0>, double>);

template<std::floating_point Float>
Float constexpr tolerance = std::same_as<Float, float> ? Float{1e-3} : Float{1e-10};


void
require(bool condition, std::string_view message)
{
  if (!condition)
  {
    throw std::runtime_error{std::string{message}};
  }
}


template<hilbert::supported_float Float>
void
test_cosine_analytic_signal()
{
  constexpr auto sample_count = 128uz;
  constexpr auto cycles = 7uz;

  std::vector<Float> input(sample_count);
  for (auto [index, sample] : std::views::enumerate(input))
  {
    auto const phase = 2 * std::numbers::pi_v<Float> * static_cast<Float>(cycles) * static_cast<Float>(index) /
                       static_cast<Float>(sample_count);
    sample = std::cos(phase);
  }

  auto const original_input = input;
  auto const analytic = hilbert::hilbert_transform(input);
  require(input == original_input, "hilbert transform modified its input");

  for (auto const &[index, sample] : std::views::enumerate(analytic))
  {
    auto const phase = 2 * std::numbers::pi_v<Float> * static_cast<Float>(cycles) * static_cast<Float>(index) /
                       static_cast<Float>(sample_count);
    auto const expected = std::polar(Float{1}, phase);
    require(std::abs(sample - expected) < tolerance<Float>, "cosine analytic signal mismatch");
  }
}


template<hilbert::supported_float Float>
void
test_instantaneous_data_for_sinusoid()
{
  constexpr auto sample_count = 256uz;
  Float constexpr sampling_rate = 256;
  Float constexpr frequency = 11;
  Float constexpr amplitude = 2.5;

  std::vector<Float> input(sample_count);
  for (auto [index, sample] : std::views::enumerate(input))
  {
    auto const phase = 2 * std::numbers::pi_v<Float> * frequency * static_cast<Float>(index) / sampling_rate;
    sample = amplitude * std::cos(phase);
  }

  auto const data = hilbert::calculate_inst_signal_data(input, sampling_rate);
  require(data.amplitude_span().size() == sample_count, "amplitude result size mismatch");
  require(data.phase_span().size() == sample_count, "phase result size mismatch");
  require(data.frequency_span().size() == sample_count, "frequency result size mismatch");

  for (auto const &[sample_amplitude, sample_frequency] : std::views::zip(data.amplitude_span(), data.frequency_span()))
  {
    require(std::abs(sample_amplitude - amplitude) < tolerance<Float>, "sinusoid amplitude mismatch");
    require(std::abs(sample_frequency - frequency) < tolerance<Float>, "sinusoid frequency mismatch");
  }
}


template<hilbert::supported_float Float>
void
test_instantaneous_frequency_preserves_negative_phase_deltas()
{
  constexpr auto sample_count = 256uz;
  Float constexpr sampling_rate = 256;

  std::vector<Float> input(sample_count);
  for (auto [index, sample] : std::views::enumerate(input))
  {
    auto const time = static_cast<Float>(index) / sampling_rate;
    sample = 2 * std::cos(2 * std::numbers::pi_v<Float> * time) + std::cos(2 * std::numbers::pi_v<Float> * 3 * time);
  }

  auto const data = hilbert::calculate_inst_signal_data(input, sampling_rate);
  auto const phase_pairs = data.phase_span() | std::views::adjacent<2>;
  auto const frequencies = data.frequency_span() | std::views::drop(1);

  bool found_negative_frequency = false;
  for (auto const &[phases, frequency] : std::views::zip(phase_pairs, frequencies))
  {
    auto const &[previous_phase, current_phase] = phases;
    auto const expected_delta = std::remainder(current_phase - previous_phase, 2 * std::numbers::pi_v<Float>);
    auto const expected_frequency = expected_delta * sampling_rate / (2 * std::numbers::pi_v<Float>);

    require(
        std::abs(frequency - expected_frequency) < tolerance<Float>,
        "instantaneous frequency used a non-principal phase delta");
    found_negative_frequency = found_negative_frequency || frequency < 0;
  }

  require(found_negative_frequency, "test signal did not produce a negative instantaneous frequency");
}


template<typename Function>
void
require_invalid_argument(Function function, std::string_view message)
{
  try
  {
    function();
  }
  catch (std::invalid_argument const &)
  {
    return;
  }

  throw std::runtime_error{std::string{message}};
}


template<hilbert::supported_float Float>
void
test_preconditions()
{
  std::vector<Float> empty;
  std::vector<Float> singleton{1};
  std::vector<Float> valid{1, -1};

  require_invalid_argument(
      [&empty]
      {
        static_cast<void>(hilbert::hilbert_transform(empty));
      },
      "empty input accepted");
  require_invalid_argument(
      [&singleton]
      {
        static_cast<void>(hilbert::hilbert_transform(singleton));
      },
      "single-sample input accepted");
  require_invalid_argument(
      [&valid]
      {
        static_cast<void>(hilbert::calculate_inst_signal_data(valid, Float{0}));
      },
      "zero sampling rate accepted");
  require_invalid_argument(
      [&valid]
      {
        static_cast<void>(hilbert::calculate_inst_signal_data(valid, std::numeric_limits<Float>::quiet_NaN()));
      },
      "non-finite sampling rate accepted");
  require_invalid_argument(
      []
      {
        static_cast<void>(hilbert::signal_data<Float>{
            std::vector<Float>{Float{1}}, std::vector<Float>{}, std::vector<Float>{Float{1}}});
      },
      "mismatched signal data channel lengths accepted");

  require_invalid_argument(
      []
      {
        using signature = hilbert::analysis::sinusoidal_signature;
        using matrix_type =
            hilbert::math::symmetric_matrix<Float, hilbert::math::dual_signature_t<signature>, signature>;
        auto const matrix =
            matrix_type::from_lower_triangle(Float{1}, Float{0}, Float{1}, Float{0}, Float{0}, Float{-1});
        static_cast<void>(hilbert::math::cholesky_decompose(matrix));
      },
      "non-positive Cholesky pivot accepted");
}


template<hilbert::supported_float Float>
void
test_tagged_sinusoidal_coefficients_ignore_order()
{
  auto const coefficients = hilbert::math::vector<Float, reordered_sinusoidal_signature>{Float{2}, Float{3}, Float{4}};
  auto const fit = hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(coefficients);

  require(fit.cosine_coefficient() == Float{3}, "tagged cosine coefficient used the positional index");
  require(fit.sine_coefficient() == Float{2}, "tagged sine coefficient used the positional index");
  require(fit.dc_offset() == Float{4}, "tagged constant coefficient used the positional index");
}


template<hilbert::supported_float Float>
void
test_precision()
{
  test_cosine_analytic_signal<Float>();
  test_instantaneous_data_for_sinusoid<Float>();
  test_instantaneous_frequency_preserves_negative_phase_deltas<Float>();
  test_preconditions<Float>();
  test_tagged_sinusoidal_coefficients_ignore_order<Float>();
}


int
run_tests()
{
  try
  {
    test_precision<double>();
  }
  catch (std::exception const &error)
  {
    std::println(std::cerr, "{}", error.what());
    return 2;
  }

  return 0;
}

} // namespace


int
main()
{
  try
  {
    return run_tests();
  }
  catch (...)
  {
    return 1;
  }
}
