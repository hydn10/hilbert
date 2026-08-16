#ifndef HILBERT_SIMULATION_SUSPENSION_SAMPLE_HPP
#define HILBERT_SIMULATION_SUSPENSION_SAMPLE_HPP


#include <concepts>


namespace hilbert::simulation::suspension
{

template<std::floating_point Float>
struct sample
{
  Float time;
  Float sprung_displacement;
  Float unsprung_displacement;
  Float ground_displacement;
  Float tire_force;
};

} // namespace hilbert::simulation::suspension

#endif // HILBERT_SIMULATION_SUSPENSION_SAMPLE_HPP
