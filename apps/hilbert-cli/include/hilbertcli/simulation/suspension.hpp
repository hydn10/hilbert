#ifndef HILBERTCLI_SIMULATION_SUSPENSION_HPP
#define HILBERTCLI_SIMULATION_SUSPENSION_HPP


#include <cmath>
#include <concepts>
#include <numbers>

#include <hilbertcli/simulation/parameters.hpp>


namespace hilbertcli
{

template<std::floating_point Float>
inline constexpr Float measurement_start_time = static_cast<Float>(7.0);

template<std::floating_point Float>
inline constexpr Float measurement_end_time = static_cast<Float>(16.0);

template<std::floating_point Float>
inline constexpr Float hilbert_margin_time = static_cast<Float>(0.2);

template<std::floating_point Float>
inline constexpr Float hilbert_start_time = measurement_start_time<Float> - hilbert_margin_time<Float>;

template<std::floating_point Float>
inline constexpr Float hilbert_end_time = measurement_end_time<Float> + hilbert_margin_time<Float>;


template<std::floating_point Float>
class state
{
  Float phi_;
  Float xs_;
  Float xu_;
  Float vs_;
  Float vu_;

public:
  state(Float phi, Float xs, Float xu, Float vs, Float vu)
      : phi_{phi}
      , xs_{xs}
      , xu_{xu}
      , vs_{vs}
      , vu_{vu}
  {
  }

  Float
  phi() const
  {
    return phi_;
  }

  Float
  xs() const
  {
    return xs_;
  }

  Float
  xu() const
  {
    return xu_;
  }

  Float
  vs() const
  {
    return vs_;
  }

  Float
  vu() const
  {
    return vu_;
  }
};


template<std::floating_point Float>
class derivative
{
  Float vphi_;
  Float vxs_;
  Float vxu_;
  Float vvs_;
  Float vvu_;

public:
  derivative(Float vphi, Float vxs, Float vxu, Float vvs, Float vvu)
      : vphi_{vphi}
      , vxs_{vxs}
      , vxu_{vxu}
      , vvs_{vvs}
      , vvu_{vvu}
  {
  }

  Float
  vphi() const
  {
    return vphi_;
  }

  Float
  vxs() const
  {
    return vxs_;
  }

  Float
  vxu() const
  {
    return vxu_;
  }

  Float
  vvs() const
  {
    return vvs_;
  }

  Float
  vvu() const
  {
    return vvu_;
  }

  derivative
  operator+(derivative const &other) const
  {
    return derivative{vphi_ + other.vphi_, vxs_ + other.vxs_, vxu_ + other.vxu_, vvs_ + other.vvs_, vvu_ + other.vvu_};
  }

  derivative
  operator*(Float scalar) const
  {
    return derivative{vphi_ * scalar, vxs_ * scalar, vxu_ * scalar, vvs_ * scalar, vvu_ * scalar};
  }
};


template<std::floating_point Float>
state<Float>
operator+(state<Float> const &current_state, derivative<Float> const &state_delta)
{
  return state{
      current_state.phi() + state_delta.vphi(),
      current_state.xs() + state_delta.vxs(),
      current_state.xu() + state_delta.vxu(),
      current_state.vs() + state_delta.vvs(),
      current_state.vu() + state_delta.vvu(),
  };
}


template<std::floating_point Float>
derivative<Float>
operator*(Float scalar, derivative<Float> const &state_delta)
{
  return state_delta * scalar;
}


template<std::floating_point Float>
constexpr Float
ground_frequency(Float time)
{
  Float constexpr summit_time = 1.5;
  Float constexpr descent_time = 6;
  Float constexpr test_end_time = 18.5;

  Float constexpr start_frequency = 0;
  Float constexpr summit_frequency = 25;
  Float constexpr measure_start_frequency = 18;
  Float constexpr measure_end_frequency = 6;
  Float constexpr end_frequency = 0;

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
      make_slope(descent_time, measurement_start_time<Float>, summit_frequency, measure_start_frequency);
  auto constexpr measure_slope = make_slope(
      measurement_start_time<Float>, measurement_end_time<Float>, measure_start_frequency, measure_end_frequency);
  auto constexpr wind_down_slope =
      make_slope(measurement_end_time<Float>, test_end_time, measure_end_frequency, end_frequency);

  if (time < summit_time)
  {
    return initial_slope(time);
  }
  if (time < descent_time)
  {
    return summit_frequency;
  }
  if (time < measurement_start_time<Float>)
  {
    return descent_slope(time);
  }
  if (time < measurement_end_time<Float>)
  {
    return measure_slope(time);
  }
  if (time < test_end_time)
  {
    return wind_down_slope(time);
  }
  return end_frequency;
}


// A profile is a callable object rather than a function pointer. The concrete profile type can therefore be known by
// the compiler and inlined into each derivative evaluation.
template<std::floating_point Float>
class scheduled_ground_frequency
{
public:
  constexpr Float
  operator()(Float time) const
  {
    return ground_frequency(time);
  }
};


template<std::floating_point Float>
class constant_ground_frequency
{
  Float frequency_hz_;

public:
  constant_ground_frequency(Float frequency_hz)
      : frequency_hz_{frequency_hz}
  {
  }

  constexpr Float
  operator()([[maybe_unused]] Float time) const
  {
    return static_cast<Float>(frequency_hz_);
  }
};


template<typename Function, typename Float>
concept ground_frequency_profile = std::floating_point<Float> && requires(Function const &function, Float time) {
  { function(time) } -> std::convertible_to<Float>;
};


template<std::floating_point Float>
constexpr auto
make_ground_position_function(Float amplitude)
{
  return [amplitude](Float phase)
  {
    return amplitude * std::sin(phase);
  };
}


template<std::floating_point Float, typename GroundPositionFunction, ground_frequency_profile<Float> FrequencyProfile>
requires requires(
    GroundPositionFunction const &ground_position_function, FrequencyProfile const &frequency_profile, Float value) {
  { ground_position_function(value) } -> std::convertible_to<Float>;
}
constexpr auto
make_state_derivative_function(
    simulation_parameters<Float> const &parameters,
    GroundPositionFunction ground_position_function,
    FrequencyProfile frequency_profile)
{
  return [=](Float time, state<Float> const &current_state) -> derivative<Float>
  {
    Float const phase_velocity = 2 * std::numbers::pi_v<Float> * static_cast<Float>(frequency_profile(time));
    Float const platform_position = ground_position_function(current_state.phi());
    Float const sprung_acceleration =
        (-parameters.suspension_damping_coefficient * (current_state.vs() - current_state.vu()) -
         parameters.suspension_spring_constant * (current_state.xs() - current_state.xu())) /
        parameters.sprung_mass;
    Float const unsprung_acceleration =
        (parameters.suspension_damping_coefficient * (current_state.vs() - current_state.vu()) +
         parameters.suspension_spring_constant * (current_state.xs() - current_state.xu()) -
         parameters.tire_spring_constant * (current_state.xu() - platform_position)) /
        parameters.unsprung_mass;

    return derivative{
        phase_velocity,
        current_state.vs(),
        current_state.vu(),
        sprung_acceleration,
        unsprung_acceleration,
    };
  };
}

} // namespace hilbertcli

#endif // HILBERTCLI_SIMULATION_SUSPENSION_HPP
