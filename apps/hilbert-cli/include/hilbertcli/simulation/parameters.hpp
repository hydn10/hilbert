#ifndef HILBERTCLI_SIMULATION_PARAMETERS_HPP
#define HILBERTCLI_SIMULATION_PARAMETERS_HPP


#include <concepts>


namespace hilbertcli
{

// Physical values are supplied by the caller. They are intentionally kept separate from integration settings so a
// future CLI/configuration parser can populate them without changing the simulation engine.
template<std::floating_point Float>
struct simulation_parameters
{
  Float sprung_mass;                    // kg
  Float unsprung_mass;                  // kg
  Float suspension_spring_constant;     // N/m
  Float suspension_damping_coefficient; // N*s/m
  Float tire_spring_constant;           // N/m
  Float ground_amplitude;               // m
};

} // namespace hilbertcli

#endif // HILBERTCLI_SIMULATION_PARAMETERS_HPP
