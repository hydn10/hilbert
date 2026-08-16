#ifndef HILBERT_SIMULATION_SUSPENSION_GROUND_FREQUENCIES_CONSTANT_HPP
#define HILBERT_SIMULATION_SUSPENSION_GROUND_FREQUENCIES_CONSTANT_HPP


#include <concepts>


namespace hilbert::simulation::suspension::ground_frequencies
{

template<std::floating_point Float>
class constant
{
  Float frequency_hz_;

public:
  explicit constant(Float frequency_hz);

  constexpr Float
  operator()([[maybe_unused]] Float time) const;
};


template<std::floating_point Float>
constant<Float>::constant(Float frequency_hz)
    : frequency_hz_{frequency_hz}
{
}


template<std::floating_point Float>
constexpr Float
constant<Float>::operator()([[maybe_unused]] Float time) const
{
  return frequency_hz_;
}

} // namespace hilbert::simulation::suspension::ground_frequencies

#endif // HILBERT_SIMULATION_SUSPENSION_GROUND_FREQUENCIES_CONSTANT_HPP
