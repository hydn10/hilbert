#ifndef HILBERT_EXAMPLE_HPP
#define HILBERT_EXAMPLE_HPP


#include <complex>
#include <vector>


namespace hilbert
{

void
hilbert_transform(std::vector<double> const &input, std::vector<std::complex<double>> &output);

void
inst_freq_amp(std::vector<double> const &data, std::vector<double> &freq, std::vector<double> &amp, double dt);

} // namespace hilbert

#endif // HILBERT_EXAMPLE_HPP
