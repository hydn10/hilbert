#ifndef HILBERT_EXAMPLE_HPP
#define HILBERT_EXAMPLE_HPP


#include <hilbert/fft.hpp>

#include <algorithm>
#include <complex>
#include <concepts>
#include <numbers>
#include <ranges>
#include <span>
#include <vector>


namespace hilbert
{

template<std::floating_point Float>
struct signal_data
{
  std::vector<Float> ampl;
  std::vector<Float> phase;
  std::vector<Float> freq;

  signal_data(size_t size);
};

std::vector<std::complex<double>>
hilbert_transform(std::span<double const> input);

signal_data<double>
calculate_inst_signal_data(std::span<double const> data, double sampling_rate);


template<std::floating_point Float>
signal_data<Float>::signal_data(size_t size)
    : ampl(size)
    , phase(size)
    , freq(size)
{
}

} // namespace hilbert

#endif
