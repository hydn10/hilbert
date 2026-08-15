#ifndef HILBERT_SIMULATION_FREQUENCY_PROFILE_HPP
#define HILBERT_SIMULATION_FREQUENCY_PROFILE_HPP


#include <concepts>


namespace hilbert::simulation::frequency
{

template<typename Function, typename Float>
concept profile = std::floating_point<Float> && requires(Function const &function, Float time) {
  { function(time) } -> std::convertible_to<Float>;
};

} // namespace hilbert::simulation::frequency

#endif // HILBERT_SIMULATION_FREQUENCY_PROFILE_HPP
