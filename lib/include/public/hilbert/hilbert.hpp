#ifndef HILBERT_HILBERT_HPP
#define HILBERT_HILBERT_HPP


#include <complex>
#include <concepts>
#include <cstddef>
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

  explicit signal_data(size_t size);
};


// At least two samples are required. Throws std::invalid_argument otherwise.
std::vector<std::complex<double>>
hilbert_transform(std::span<double const> input);


// At least two samples and a finite, positive sampling rate are required.
// Throws std::invalid_argument when either precondition is not satisfied.
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
