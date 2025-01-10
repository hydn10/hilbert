#ifndef HILBERT_EXAMPLE_HPP
#define HILBERT_EXAMPLE_HPP


#include <complex>
#include <concepts>
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
hilbert_transform(std::vector<double> const &input);

signal_data<double>
calculate_inst_signal_data(std::vector<double> const &data, double sampling_rate);


template<std::floating_point Float>
signal_data<Float>::signal_data(size_t size)
    : ampl(size)
    , phase(size)
    , freq(size)
{
}

} // namespace hilbert

#endif
