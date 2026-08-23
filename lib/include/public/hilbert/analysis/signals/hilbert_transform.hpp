#ifndef HILBERT_ANALYSIS_SIGNALS_HILBERT_TRANSFORM_HPP
#define HILBERT_ANALYSIS_SIGNALS_HILBERT_TRANSFORM_HPP


#include <hilbert/core/supported_float.hpp>

#include <complex>
#include <span>
#include <vector>


namespace hilbert
{

// At least two samples are required. Throws std::invalid_argument otherwise.
template<supported_float Float>
std::vector<std::complex<Float>>
hilbert_transform(std::span<Float const> input);


template<supported_float Float, typename Allocator>
std::vector<std::complex<Float>>
hilbert_transform(std::vector<Float, Allocator> const &input)
{
  return hilbert_transform<Float>(std::span<Float const>{input});
}


extern template std::vector<std::complex<double>>
hilbert_transform<double>(std::span<double const> input);

} // namespace hilbert

#endif // HILBERT_ANALYSIS_SIGNALS_HILBERT_TRANSFORM_HPP
