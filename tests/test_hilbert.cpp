#include <hilbert/hilbert.hpp>

#include <cmath>
#include <complex>
#include <cstddef>
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

constexpr double tolerance = 1e-10;


void
require(bool condition, std::string_view message)
{
  if (!condition)
  {
    throw std::runtime_error{std::string{message}};
  }
}


void
test_cosine_analytic_signal()
{
  constexpr size_t sample_count = 128;
  constexpr size_t cycles = 7;

  std::vector<double> input(sample_count);
  for (auto [index, sample] : std::views::enumerate(input))
  {
    auto const phase = 2.0 * std::numbers::pi * static_cast<double>(cycles) * static_cast<double>(index) / sample_count;
    sample = std::cos(phase);
  }

  auto const original_input = input;
  auto const analytic = hilbert::hilbert_transform(input);
  require(input == original_input, "hilbert transform modified its input");

  for (auto const &[index, sample] : std::views::enumerate(analytic))
  {
    auto const phase = 2.0 * std::numbers::pi * static_cast<double>(cycles) * static_cast<double>(index) / sample_count;
    auto const expected = std::polar(1.0, phase);
    require(std::abs(sample - expected) < tolerance, "cosine analytic signal mismatch");
  }
}


void
test_instantaneous_data_for_sinusoid()
{
  constexpr size_t sample_count = 256;
  constexpr double sampling_rate = 256.0;
  constexpr double frequency = 11.0;
  constexpr double amplitude = 2.5;

  std::vector<double> input(sample_count);
  for (auto [index, sample] : std::views::enumerate(input))
  {
    auto const phase = 2.0 * std::numbers::pi * frequency * static_cast<double>(index) / sampling_rate;
    sample = amplitude * std::cos(phase);
  }

  auto const data = hilbert::calculate_inst_signal_data(input, sampling_rate);
  require(data.ampl.size() == sample_count, "amplitude result size mismatch");
  require(data.phase.size() == sample_count, "phase result size mismatch");
  require(data.freq.size() == sample_count, "frequency result size mismatch");

  for (auto const &[sample_amplitude, sample_frequency] : std::views::zip(data.ampl, data.freq))
  {
    require(std::abs(sample_amplitude - amplitude) < tolerance, "sinusoid amplitude mismatch");
    require(std::abs(sample_frequency - frequency) < tolerance, "sinusoid frequency mismatch");
  }
}


void
test_instantaneous_frequency_preserves_negative_phase_deltas()
{
  constexpr size_t sample_count = 256;
  constexpr double sampling_rate = 256.0;

  std::vector<double> input(sample_count);
  for (auto [index, sample] : std::views::enumerate(input))
  {
    auto const time = static_cast<double>(index) / sampling_rate;
    sample = 2.0 * std::cos(2.0 * std::numbers::pi * time) + std::cos(2.0 * std::numbers::pi * 3.0 * time);
  }

  auto const data = hilbert::calculate_inst_signal_data(input, sampling_rate);
  auto const phase_pairs = data.phase | std::views::adjacent<2>;
  auto const frequencies = data.freq | std::views::drop(1);

  bool found_negative_frequency = false;
  for (auto const &[phases, frequency] : std::views::zip(phase_pairs, frequencies))
  {
    auto const &[previous_phase, current_phase] = phases;
    auto const expected_delta = std::remainder(current_phase - previous_phase, 2.0 * std::numbers::pi);
    auto const expected_frequency = expected_delta * sampling_rate / (2.0 * std::numbers::pi);

    require(
        std::abs(frequency - expected_frequency) < tolerance,
        "instantaneous frequency used a non-principal phase delta");
    found_negative_frequency = found_negative_frequency || frequency < 0.0;
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


void
test_preconditions()
{
  std::vector<double> empty;
  std::vector<double> singleton{1.0};
  std::vector<double> valid{1.0, -1.0};

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
        static_cast<void>(hilbert::calculate_inst_signal_data(valid, 0.0));
      },
      "zero sampling rate accepted");
  require_invalid_argument(
      [&valid]
      {
        static_cast<void>(hilbert::calculate_inst_signal_data(valid, std::numeric_limits<double>::quiet_NaN()));
      },
      "non-finite sampling rate accepted");
}


int
run_tests()
{
  try
  {
    test_cosine_analytic_signal();
    test_instantaneous_data_for_sinusoid();
    test_instantaneous_frequency_preserves_negative_phase_deltas();
    test_preconditions();
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
