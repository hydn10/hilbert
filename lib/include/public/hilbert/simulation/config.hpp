#ifndef HILBERT_SIMULATION_CONFIG_HPP
#define HILBERT_SIMULATION_CONFIG_HPP


#include <concepts>


namespace hilbert::simulation
{

template<std::floating_point Float>
struct simulation_settings
{
  Float time_step;
  Float duration;
};

template<std::floating_point Float>
struct suspension_parameters
{
  Float sprung_mass;                    // kg
  Float unsprung_mass;                  // kg
  Float suspension_spring_constant;     // N/m
  Float suspension_damping_coefficient; // N*s/m
  Float tire_spring_constant;           // N/m
  Float ground_amplitude;               // m
};

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_CONFIG_HPP
