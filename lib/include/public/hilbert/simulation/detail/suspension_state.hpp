#ifndef HILBERT_SIMULATION_DETAIL_SUSPENSION_STATE_HPP
#define HILBERT_SIMULATION_DETAIL_SUSPENSION_STATE_HPP


#include <concepts>


namespace hilbert::simulation::detail
{

template<std::floating_point Float>
class suspension_state
{
  Float phase_;
  Float sprung_displacement_;
  Float unsprung_displacement_;
  Float sprung_velocity_;
  Float unsprung_velocity_;

public:
  suspension_state(
      Float phase,
      Float sprung_displacement,
      Float unsprung_displacement,
      Float sprung_velocity,
      Float unsprung_velocity)
      : phase_{phase}
      , sprung_displacement_{sprung_displacement}
      , unsprung_displacement_{unsprung_displacement}
      , sprung_velocity_{sprung_velocity}
      , unsprung_velocity_{unsprung_velocity}
  {
  }

  Float
  phase() const
  {
    return phase_;
  }

  Float
  sprung_displacement() const
  {
    return sprung_displacement_;
  }

  Float
  unsprung_displacement() const
  {
    return unsprung_displacement_;
  }

  Float
  sprung_velocity() const
  {
    return sprung_velocity_;
  }

  Float
  unsprung_velocity() const
  {
    return unsprung_velocity_;
  }
};


template<std::floating_point Float>
class suspension_derivative
{
  Float phase_velocity_;
  Float sprung_displacement_velocity_;
  Float unsprung_displacement_velocity_;
  Float sprung_acceleration_;
  Float unsprung_acceleration_;

public:
  suspension_derivative(
      Float phase_velocity,
      Float sprung_displacement_velocity,
      Float unsprung_displacement_velocity,
      Float sprung_acceleration,
      Float unsprung_acceleration)
      : phase_velocity_{phase_velocity}
      , sprung_displacement_velocity_{sprung_displacement_velocity}
      , unsprung_displacement_velocity_{unsprung_displacement_velocity}
      , sprung_acceleration_{sprung_acceleration}
      , unsprung_acceleration_{unsprung_acceleration}
  {
  }

  Float
  phase_velocity() const
  {
    return phase_velocity_;
  }

  Float
  sprung_displacement_velocity() const
  {
    return sprung_displacement_velocity_;
  }

  Float
  unsprung_displacement_velocity() const
  {
    return unsprung_displacement_velocity_;
  }

  Float
  sprung_acceleration() const
  {
    return sprung_acceleration_;
  }

  Float
  unsprung_acceleration() const
  {
    return unsprung_acceleration_;
  }

  suspension_derivative
  operator+(suspension_derivative const &other) const
  {
    return {
        phase_velocity_ + other.phase_velocity_,
        sprung_displacement_velocity_ + other.sprung_displacement_velocity_,
        unsprung_displacement_velocity_ + other.unsprung_displacement_velocity_,
        sprung_acceleration_ + other.sprung_acceleration_,
        unsprung_acceleration_ + other.unsprung_acceleration_,
    };
  }

  suspension_derivative
  operator*(Float scalar) const
  {
    return {
        phase_velocity_ * scalar,
        sprung_displacement_velocity_ * scalar,
        unsprung_displacement_velocity_ * scalar,
        sprung_acceleration_ * scalar,
        unsprung_acceleration_ * scalar,
    };
  }
};


template<std::floating_point Float>
suspension_state<Float>
operator+(suspension_state<Float> const &state, suspension_derivative<Float> const &delta)
{
  return {
      state.phase() + delta.phase_velocity(),
      state.sprung_displacement() + delta.sprung_displacement_velocity(),
      state.unsprung_displacement() + delta.unsprung_displacement_velocity(),
      state.sprung_velocity() + delta.sprung_acceleration(),
      state.unsprung_velocity() + delta.unsprung_acceleration(),
  };
}


template<std::floating_point Float>
suspension_derivative<Float>
operator*(Float scalar, suspension_derivative<Float> const &delta)
{
  return delta * scalar;
}

} // namespace hilbert::simulation::detail

#endif // HILBERT_SIMULATION_DETAIL_SUSPENSION_STATE_HPP
