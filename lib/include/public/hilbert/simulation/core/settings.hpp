#ifndef HILBERT_SIMULATION_CORE_SETTINGS_HPP
#define HILBERT_SIMULATION_CORE_SETTINGS_HPP


#include <concepts>


namespace hilbert::simulation
{

template<std::floating_point Float>
struct simulation_settings
{
  Float time_step;
  Float duration;
};

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_CORE_SETTINGS_HPP
