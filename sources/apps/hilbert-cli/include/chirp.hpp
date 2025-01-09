#ifndef HILBERTCLI_CHIRP_HPP
#define HILBERTCLI_CHIRP_HPP


#include <cmath>
#include <concepts>
#include <numbers>
#include <ranges>
#include <vector>


namespace hilbertcli
{

template<std::floating_point Float>
std::vector<Float>
generate_chirp(Float frequency_start, Float frequency_end, Float duration, uint32_t sampling_rate)
{
  auto const num_samples = static_cast<uint32_t>(duration * sampling_rate);

  auto const fs = frequency_start;
  auto const fe = frequency_end;
  auto const d = duration;
  auto const s = sampling_rate;

  auto const r = std::views::iota(uint32_t{0}, num_samples) |
                 std::views::transform(
                     [fs, fe, d, s](auto idx)
                     {
                       // Instantaneous frequency is the derivative of the phase.
                       // Here we are calculating the phase integrating from the desired frequency curve.

                       auto const t = static_cast<Float>(idx) / s; // Time in seconds.
                       auto const f_equiv = fs + (fe - fs) * t * 0.5 / d;
                       auto const chirp_signal = std::sin(2 * std::numbers::pi * t * f_equiv);

                       return chirp_signal;
                     });

  return std::vector<Float>(r.begin(), r.end());
}

} // namespace hilbertcli

#endif // HILBERTCLI_CHIRP_HPP
