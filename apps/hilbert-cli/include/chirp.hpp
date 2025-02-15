#ifndef HILBERTCLI_CHIRP_HPP
#define HILBERTCLI_CHIRP_HPP


#include <cmath>
#include <concepts>
#include <cstdint>
#include <numbers>
#include <ranges>
#include <vector>


namespace hilbertcli
{

template<std::floating_point Float>
auto
generate_chirp(Float frequency_start, Float frequency_end, Float duration, uint32_t sampling_rate)
{
  auto const num_samples = static_cast<uint32_t>(duration * sampling_rate);

  auto const fs = frequency_start;
  auto const fe = frequency_end;
  auto const d = duration;
  auto const s = sampling_rate;

  return std::views::iota(uint32_t{0}, num_samples) | std::views::transform([fs, fe, d, s](auto idx)
  {
    // Instantaneous angular velocity is the derivative of the phase.
    // The frequency is the angular velocity normalized by 2 * pi.
    // Here we are calculating the phase integrating from the desired frequency curve.

    auto const t = static_cast<Float>(idx) / s;
    auto const f_equiv = fs + (fe - fs) * t * 0.5 / d;
    auto const chirp_signal = std::sin(2 * std::numbers::pi * t * f_equiv);

    return chirp_signal;
  });
}

} // namespace hilbertcli

#endif // HILBERTCLI_CHIRP_HPP
