#ifndef HILBERT_SIMULATION_SUSPENSION_FREQUENCY_HPP
#define HILBERT_SIMULATION_SUSPENSION_FREQUENCY_HPP


#include <concepts>


namespace hilbert::simulation::suspension::ground_frequency
{

template<typename Function, typename Float>
concept profile = std::floating_point<Float> && requires(Function const &function, Float time) {
  { function(time) } -> std::convertible_to<Float>;
};

} // namespace hilbert::simulation::suspension::ground_frequency

#endif // HILBERT_SIMULATION_SUSPENSION_FREQUENCY_HPP
