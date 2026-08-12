#include <hilbert/hilbert.hpp>

#include <cmath>
#include <numbers>
#include <vector>

int
main()
{
  std::vector<double> samples(32uz);
  for (auto index = 0uz; index < samples.size(); ++index)
  {
    samples[index] = std::cos(2.0 * std::numbers::pi * static_cast<double>(index) / samples.size());
  }

  auto const analytic = hilbert::hilbert_transform(samples);
  return analytic.size() == samples.size() ? 0 : 1;
}
