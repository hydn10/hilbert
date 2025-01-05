#include <hilbert/hilbert.hpp>

#include <chirp.hpp>

#include <print>


int
main()
{
  double constexpr DURATION = 3;
  uint32_t constexpr SAMPLING_RATE = 5000;

  auto const data = hilbertcli::generate_chirp(20., 100., DURATION, SAMPLING_RATE);

  auto const num_samples = data.size();

  std::vector<double> amp(num_samples);
  std::vector<double> phase(num_samples);
  std::vector<double> freq(num_samples);

  hilbert::inst_amp_phase_freq(data, amp, phase, freq, SAMPLING_RATE);

  int const index_width = static_cast<int>(std::log10(num_samples)) + 1;

  int constexpr field_with = 12;
  int constexpr precision = 6;

  auto const format_number = [field_with, precision](double value)
  {
    return std::format("{:>{}.{}f}", value, field_with, precision);
  };

  auto format_index = [index_width](std::size_t index)
  {
    return std::format("{:>{}}", index, index_width);
  };

  for (size_t i = 0; i < num_samples; ++i)
  {
    std::println(
        "{} {} {} {} {}",
        format_index(i),
        format_number(data[i]),
        format_number(amp[i]),
        format_number(phase[i]),
        format_number(freq[i]));
  }
}
