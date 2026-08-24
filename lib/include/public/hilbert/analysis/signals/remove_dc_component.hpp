#ifndef HILBERT_ANALYSIS_SIGNALS_REMOVE_DC_COMPONENT_HPP
#define HILBERT_ANALYSIS_SIGNALS_REMOVE_DC_COMPONENT_HPP


#include <hilbert/core/supported_float.hpp>

#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>


namespace hilbert::analysis
{

template<supported_float Float>
std::vector<Float>
remove_dc_component(std::span<Float const> signal);


template<supported_float Float>
std::vector<Float>
remove_dc_component(std::span<Float const> signal)
{
  if (signal.empty())
  {
    throw std::invalid_argument{"cannot center an empty signal"};
  }

  Float sum = 0;
  for (auto const value : signal)
  {
    sum += value;
  }

  auto const mean = sum / static_cast<Float>(signal.size());
  std::vector<Float> centered(signal.size());
  for (auto [value, centered_value] : std::views::zip(signal, centered))
  {
    centered_value = value - mean;
  }
  return centered;
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SIGNALS_REMOVE_DC_COMPONENT_HPP
