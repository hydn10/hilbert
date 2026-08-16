#ifndef HILBERT_SIMULATION_SUSPENSION_PARAMETERS_HPP
#define HILBERT_SIMULATION_SUSPENSION_PARAMETERS_HPP


#include <concepts>


namespace hilbert::simulation::suspension
{

template<std::floating_point Float>
struct parameters
{
  Float sprung_mass;                    // kg
  Float unsprung_mass;                  // kg
  Float suspension_spring_constant;     // N/m
  Float suspension_damping_coefficient; // N*s/m
  Float tire_spring_constant;           // N/m
  Float ground_amplitude;               // m
};

} // namespace hilbert::simulation::suspension

#endif // HILBERT_SIMULATION_SUSPENSION_PARAMETERS_HPP
