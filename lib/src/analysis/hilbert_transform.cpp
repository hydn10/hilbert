#include <hilbert/analysis/signals/hilbert_transform.hpp>

#include <hilbert/core/supported_float.hpp>
#include <hilbert/detail/fft.hpp>

#include <algorithm>
#include <complex>
#include <cstddef>
#include <limits>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>


namespace hilbert
{
namespace
{

auto
take_positive_frequencies(std::size_t size)
{
  return std::views::drop(1) | std::views::take((size - 1uz) / 2uz);
}


auto
take_negative_frequencies(std::size_t size)
{
  return std::views::reverse | std::views::take((size - 1uz) / 2uz);
}


inline constexpr auto double_frequency_bin = [](auto &value)
{
  value *= 2;
};


inline constexpr auto clear_frequency_bin = [](auto &value)
{
  value = 0;
};

} // namespace


template<supported_float Float>
std::vector<std::complex<Float>>
hilbert_transform(std::span<Float const> input)
{
  auto const size = input.size();

  if (size < 2uz)
  {
    throw std::invalid_argument{"hilbert_transform requires at least two samples"};
  }

  if (size > static_cast<std::size_t>(std::numeric_limits<int>::max()))
  {
    throw std::length_error{"hilbert_transform input exceeds FFTW's one-dimensional size limit"};
  }

  auto frequency_data = detail::fft::transform(input);

  std::ranges::for_each(frequency_data | take_positive_frequencies(size), double_frequency_bin);
  std::ranges::for_each(frequency_data | take_negative_frequencies(size), clear_frequency_bin);

  auto time_data =
      detail::fft::transform(std::span<std::complex<Float> const>{frequency_data}, detail::fft::sign::backward);

  std::ranges::for_each(
      time_data,
      [size](auto &value)
      {
        value /= static_cast<Float>(size);
      });

  return time_data;
}


template std::vector<std::complex<double>>
hilbert_transform<double>(std::span<double const> input);

} // namespace hilbert
