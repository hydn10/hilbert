#ifndef HILBERT_ANALYSIS_SIGNALS_REMOVE_DC_COMPONENT_HPP
#define HILBERT_ANALYSIS_SIGNALS_REMOVE_DC_COMPONENT_HPP


#include <hilbert/core/supported_float.hpp>

#include <algorithm>
#include <functional>
#include <ranges>
#include <span>


namespace hilbert::analysis
{

template<supported_float Float>
class dc_component
{
  Float value_;

public:
  explicit dc_component(Float value) noexcept;

  [[nodiscard]]
  Float
  value() const noexcept;
};


// Precondition: signal is non-empty and contains finite values.
template<supported_float Float>
[[nodiscard]]
dc_component<Float>
estimate_dc_component(std::span<Float const> signal);


template<supported_float Float>
[[nodiscard]]
auto
remove_dc_component(dc_component<Float> component);


template<supported_float Float>
dc_component<Float>::dc_component(Float value) noexcept
    : value_{value}
{
}


template<supported_float Float>
Float
dc_component<Float>::value() const noexcept
{
  return value_;
}


template<supported_float Float>
dc_component<Float>
estimate_dc_component(std::span<Float const> signal)
{
  auto const sum = std::ranges::fold_left(signal, Float{}, std::plus<>{});
  return dc_component<Float>{sum / static_cast<Float>(signal.size())};
}


template<supported_float Float>
auto
remove_dc_component(dc_component<Float> component)
{
  return std::bind_back(std::minus<>{}, component.value());
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SIGNALS_REMOVE_DC_COMPONENT_HPP
