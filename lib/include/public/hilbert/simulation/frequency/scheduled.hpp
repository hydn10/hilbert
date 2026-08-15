#ifndef HILBERT_SIMULATION_FREQUENCY_SCHEDULED_HPP
#define HILBERT_SIMULATION_FREQUENCY_SCHEDULED_HPP


#include <concepts>


namespace hilbert::simulation::frequency
{

template<std::floating_point Float>
class scheduled
{
public:
  constexpr Float
  operator()(Float time) const
  {
    Float constexpr summit_time = static_cast<Float>(1.5);
    Float constexpr descent_time = static_cast<Float>(6);
    Float constexpr measurement_start_time = static_cast<Float>(7);
    Float constexpr measurement_end_time = static_cast<Float>(16);
    Float constexpr test_end_time = static_cast<Float>(18.5);

    Float constexpr start_frequency = static_cast<Float>(0);
    Float constexpr summit_frequency = static_cast<Float>(25);
    Float constexpr measurement_start_frequency = static_cast<Float>(18);
    Float constexpr measurement_end_frequency = static_cast<Float>(6);
    Float constexpr end_frequency = static_cast<Float>(0);

    auto constexpr make_slope = [](Float begin_time, Float finish_time, Float initial_frequency, Float final_frequency)
    {
      return [=](Float value)
      {
        return initial_frequency +
               (final_frequency - initial_frequency) / (finish_time - begin_time) * (value - begin_time);
      };
    };

    auto constexpr initial_slope = make_slope(0, summit_time, start_frequency, summit_frequency);
    auto constexpr descent_slope =
        make_slope(descent_time, measurement_start_time, summit_frequency, measurement_start_frequency);
    auto constexpr measurement_slope = make_slope(
        measurement_start_time, measurement_end_time, measurement_start_frequency, measurement_end_frequency);
    auto constexpr wind_down_slope =
        make_slope(measurement_end_time, test_end_time, measurement_end_frequency, end_frequency);

    if (time < summit_time)
    {
      return initial_slope(time);
    }
    if (time < descent_time)
    {
      return summit_frequency;
    }
    if (time < measurement_start_time)
    {
      return descent_slope(time);
    }
    if (time < measurement_end_time)
    {
      return measurement_slope(time);
    }
    if (time < test_end_time)
    {
      return wind_down_slope(time);
    }
    return end_frequency;
  }
};

} // namespace hilbert::simulation::frequency

#endif // HILBERT_SIMULATION_FREQUENCY_SCHEDULED_HPP
