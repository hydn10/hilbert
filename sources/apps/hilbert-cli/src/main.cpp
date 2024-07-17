#include <hilbert/hilbert.hpp>

#include <chirp.hpp>

#include <iostream>


int
main()
{
  constexpr double DURATION = 1;
  constexpr uint32_t SAMPLING_RATE = 5000;

  auto const data = hilbertcli::generate_chirp(20., 60., DURATION, SAMPLING_RATE);

  auto const num_samples = data.size();

  std::vector<double> freq(num_samples);
  std::vector<double> amp(num_samples);

  hilbert::inst_freq_amp(data, freq, amp, 1. / SAMPLING_RATE);

  for (size_t i = 0; i < num_samples; ++i)
  {
    std::cout << i << ' ' << data[i] << ' ' << freq[i] << ' ' << amp[i] << '\n';
  }
}
