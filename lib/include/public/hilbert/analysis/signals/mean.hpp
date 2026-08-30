#ifndef HILBERT_ANALYSIS_SIGNALS_MEAN_HPP
#define HILBERT_ANALYSIS_SIGNALS_MEAN_HPP


#include <hilbert/core/supported_float.hpp>

#include <algorithm>
#include <functional>
#include <ranges>
#include <span>


namespace hilbert::analysis
{

// Precondition: signal is non-empty and contains finite values.
template<supported_float Float>
[[nodiscard]]
Float
mean(std::span<Float const> signal);


template<supported_float Float>
[[nodiscard]]
auto
subtract_offset(Float offset);


template<supported_float Float>
Float
mean(std::span<Float const> signal)
{
  auto const sum = std::ranges::fold_left(signal, Float{}, std::plus<>{});
  return sum / static_cast<Float>(signal.size());
}


template<supported_float Float>
auto
subtract_offset(Float offset)
{
  return std::bind_back(std::minus<>{}, offset);
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SIGNALS_MEAN_HPP
