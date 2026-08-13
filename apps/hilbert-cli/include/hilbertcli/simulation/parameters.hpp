#ifndef HILBERTCLI_SIMULATION_PARAMETERS_HPP
#define HILBERTCLI_SIMULATION_PARAMETERS_HPP


namespace hilbertcli
{

// Physical values are supplied by the caller. They are intentionally kept separate from integration settings so a
// future CLI/configuration parser can populate them without changing the simulation engine.
struct simulation_parameters
{
  double sprung_mass;                    // kg
  double unsprung_mass;                  // kg
  double suspension_spring_constant;     // N/m
  double suspension_damping_coefficient; // N*s/m
  double tire_spring_constant;           // N/m
  double ground_amplitude;               // m
};

} // namespace hilbertcli

#endif // HILBERTCLI_SIMULATION_PARAMETERS_HPP
