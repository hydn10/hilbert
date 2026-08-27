#include <hilbert/hilbert.hpp>

#include <array>
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
#include <utility>
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
static_assert(
    hilbert::math::signature_contains<hilbert::analysis::sinusoidal_signature const &, hilbert::analysis::cosine_term>);
static_assert(!hilbert::analysis::sinusoidal_coordinate_signature<dual_sinusoidal_signature>);
static_assert(hilbert::analysis::basis_for_size<sinusoidal_basis, 3uz, double>);
static_assert(
    hilbert::analysis::least_squares_observation_for<hilbert::analysis::least_squares_observation<double, 2>, double>);
static_assert(
    !hilbert::analysis::least_squares_observation_for<hilbert::analysis::least_squares_observation<double, 0>, double>);

template<std::floating_point Float>
Float constexpr tolerance = std::same_as<Float, float> ? Float{1e-3} : Float{1e-10};

template<typename Function>
auto
make_samples(auto sample_count, Function function)
{
  return std::views::iota(0uz, sample_count) | std::views::transform(std::move(function)) |
         std::ranges::to<std::vector>();
}


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

  auto const input = make_samples(
      sample_count,
      [=](auto index)
      {
        auto const phase = 2 * std::numbers::pi_v<Float> * static_cast<Float>(cycles) * static_cast<Float>(index) /
                           static_cast<Float>(sample_count);
        return std::cos(phase);
      });

  auto const analytic = hilbert::hilbert_transform(input);

  auto const expected = make_samples(
      sample_count,
      [=](auto index)
      {
        auto const phase = 2 * std::numbers::pi_v<Float> * static_cast<Float>(cycles) * static_cast<Float>(index) /
                           static_cast<Float>(sample_count);
        return std::polar(Float{1}, phase);
      });

  for (auto const &[sample, expected_sample] : std::views::zip(analytic, expected))
  {
    require(analytic.size() == expected.size(), "analytic signal size mismatch");
    require(std::abs(sample - expected_sample) < tolerance<Float>, "cosine analytic signal mismatch");
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

  auto const input = make_samples(
      sample_count,
      [=](auto index)
      {
        auto const phase = 2 * std::numbers::pi_v<Float> * frequency * static_cast<Float>(index) / sampling_rate;
        return amplitude * std::cos(phase);
      });

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

  auto const input = make_samples(
      sample_count,
      [=](auto index)
      {
        auto const time = static_cast<Float>(index) / sampling_rate;
        return 2 * std::cos(2 * std::numbers::pi_v<Float> * time) + std::cos(2 * std::numbers::pi_v<Float> * 3 * time);
      });

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
test_frequency_response()
{
  using signature = hilbert::analysis::sinusoidal_signature;

  auto const input_coefficients = hilbert::math::vector<Float, signature>{Float{1}, Float{0}, Float{0}};
  auto const output_coefficients = hilbert::math::vector<Float, signature>{Float{0}, Float{-2}, Float{0}};
  auto const input = hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(input_coefficients);
  auto const output = hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(output_coefficients);
  auto const response = hilbert::analysis::make_frequency_response(output, input);

  require(std::abs(response.magnitude() - Float{2}) < tolerance<Float>, "frequency-response magnitude mismatch");
  require(
      std::abs(response.phase().radians() - std::numbers::pi_v<Float> / 2) < tolerance<Float>,
      "frequency-response phase orientation mismatch");

  auto const zero_input = hilbert::analysis::sinusoidal_fit<Float>::from_coefficients(
      hilbert::math::vector<Float, signature>{Float{0}, Float{0}, Float{0}});
  require_invalid_argument(
      [&output, &zero_input]
      {
        static_cast<void>(hilbert::analysis::make_frequency_response(output, zero_input));
      },
      "zero frequency-response input accepted");
}


void
test_least_squares_residual_statistics()
{
  hilbert::analysis::least_squares_residual_statistics_collector<double, 1uz> collector;
  constexpr double offset = 1e12;
  collector.observe(std::array{offset + 1});
  collector.observe(std::array{offset + 2});
  collector.observe(std::array{offset + 3});
  auto const statistics = std::move(collector).finish(3uz);

  require(statistics.response_means.at(0) == offset + 2, "online response mean mismatch");
  require(statistics.response_centered_squared_norms.at(0) == 2, "online centered response energy mismatch");
}


void
test_full_least_squares_residual_objective()
{
  using signature = hilbert::analysis::sinusoidal_signature;
  using matrix_type = hilbert::math::symmetric_matrix<double, hilbert::math::dual_signature_t<signature>, signature>;

  auto const gram = matrix_type::from_lower_triangle(1.0, 0.0, 1.0, 0.0, 0.0, 1.0);
  auto const projection = hilbert::math::vector<double, hilbert::math::dual_signature_t<signature>>{1.0, 2.0, 3.0};
  auto const coefficients = hilbert::math::vector<double, signature>{0.9, 2.2, 2.7};
  auto const residual =
      hilbert::analysis::normalized_least_squares_residual(gram, projection, coefficients, 14.0l, 2.0l);

  require(std::abs(residual - std::sqrt(0.07)) < tolerance<double>, "full residual objective mismatch");
}


void
test_basis_reciprocal_conditioning()
{
  using signature = hilbert::analysis::sinusoidal_signature;
  using matrix_type = hilbert::math::symmetric_matrix<double, hilbert::math::dual_signature_t<signature>, signature>;

  auto const identity = matrix_type::from_lower_triangle(1.0, 0.0, 1.0, 0.0, 0.0, 1.0);
  require(
      hilbert::analysis::column_normalized_basis_reciprocal_condition_number(identity) == 1,
      "identity basis reciprocal condition mismatch");

  auto const singular = matrix_type::from_lower_triangle(1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
  require(
      hilbert::analysis::column_normalized_basis_reciprocal_condition_number(singular) == 0,
      "singular basis reciprocal condition was nonzero");
  require(
      std::isinf(hilbert::analysis::column_normalized_basis_condition_number(singular)),
      "singular basis condition number was finite");
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
  test_frequency_response<Float>();
  test_least_squares_residual_statistics();
  test_full_least_squares_residual_objective();
  test_basis_reciprocal_conditioning();
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
