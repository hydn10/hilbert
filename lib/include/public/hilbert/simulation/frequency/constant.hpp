#ifndef HILBERT_SIMULATION_FREQUENCY_CONSTANT_HPP
#define HILBERT_SIMULATION_FREQUENCY_CONSTANT_HPP


#include <concepts>


namespace hilbert::simulation::frequency
{

template<std::floating_point Float>
class constant
{
  Float frequency_hz_;

public:
  explicit constant(Float frequency_hz)
      : frequency_hz_{frequency_hz}
  {
  }

  constexpr Float
  operator()([[maybe_unused]] Float time) const
  {
    return frequency_hz_;
  }
};

} // namespace hilbert::simulation::frequency

#endif // HILBERT_SIMULATION_FREQUENCY_CONSTANT_HPP
