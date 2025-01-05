#ifndef HILBERT_EXAMPLE_HPP
#define HILBERT_EXAMPLE_HPP


#include <complex>
#include <vector>


namespace hilbert
{

std::vector<std::complex<double>>
hilbert_transform(std::vector<double> const &input);

void
inst_amp_phase_freq(
    std::vector<double> const &data,
    std::vector<double> &amp,
    std::vector<double> &phase,
    std::vector<double> &freq,
    double dt);

} // namespace hilbert

#endif // HILBERT_EXAMPLE_HPP
