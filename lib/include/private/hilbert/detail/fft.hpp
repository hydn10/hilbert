#ifndef HILBERT_DETAIL_FFT_HPP
#define HILBERT_DETAIL_FFT_HPP


#include <hilbert/core/supported_float.hpp>

#include <complex>
#include <cstdint>
#include <span>
#include <vector>


namespace hilbert::detail::fft
{

enum class sign : std::uint8_t
{
  forward,
  backward,
};


template<supported_float Float>
std::vector<std::complex<Float>>
transform(std::span<Float const> input);


template<supported_float Float>
std::vector<std::complex<Float>>
transform(std::span<std::complex<Float> const> input, sign direction);

} // namespace hilbert::detail::fft

#endif
