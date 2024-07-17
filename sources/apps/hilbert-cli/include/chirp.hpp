#ifndef HILBERTCLI_CHIRP_HPP
#define HILBERTCLI_CHIRP_HPP


#include <ranges>
#include <vector>
#include <numbers>
#include <cmath>


namespace hilbertcli
{

template<typename Float>
std::vector<Float>
generate_chirp(Float frequency_start, Float frequency_end, Float duration, uint32_t sampling_rate)
{
  size_t num_samples = static_cast<size_t>(duration * sampling_rate);

  auto r = std::views::iota((size_t)0, num_samples)
      | std::views::transform([&](size_t idx)
      {
        Float t = static_cast<Float>(idx) / sampling_rate; // Time in seconds
        Float frequency = frequency_start + (frequency_end - frequency_start) * t / duration;
        Float chirp_signal = std::sin(2 * std::numbers::pi * frequency * t);

        return chirp_signal;
      });

  return std::vector<Float>(r.begin(), r.end());
}

} // namespace hilbertcli

#endif // HILBERTCLI_CHIRP_HPP
