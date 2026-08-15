#ifndef HILBERT_SIMULATION_CONFIG_HPP
#define HILBERT_SIMULATION_CONFIG_HPP


#include <concepts>


namespace hilbert::simulation
{

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


template<std::floating_point Float>
struct config
{
  Float time_step = static_cast<Float>(0.0005);
  Float duration = static_cast<Float>(20.0);
  suspension_parameters<Float> parameters;
};

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_CONFIG_HPP
