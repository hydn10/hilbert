#include <chirp.hpp>
#include <derivative.hpp>
#include <hilbert/hilbert.hpp>
#include <rk4.hpp>
#include <state.hpp>

#include <cmath>
#include <print>


void
hilbert_example()
{
  double constexpr DURATION = 3;
  uint32_t constexpr SAMPLING_RATE = 5000;

  auto const data = hilbertcli::generate_chirp(20., 100., DURATION, SAMPLING_RATE);
  auto const num_samples = data.size();

  auto const signal_data = hilbert::calculate_inst_signal_data(data, SAMPLING_RATE);

  auto const index_width = static_cast<int>(std::log10(num_samples)) + 1;

  auto constexpr field_with = 12;
  auto constexpr precision = 6;

  auto const format_number = [field_with, precision](double value)
  {
    return std::format("{:>{}.{}f}", value, field_with, precision);
  };

  auto format_index = [index_width](std::size_t index)
  {
    return std::format("{:>{}}", index, index_width);
  };

  for (size_t i = 0; i < num_samples; ++i)
  {
    std::println(
        "{} {} {} {} {}",
        format_index(i),
        format_number(data[i]),
        format_number(signal_data.ampl[i]),
        format_number(signal_data.phase[i]),
        format_number(signal_data.freq[i]));
  }
}


template<std::floating_point Float>
static Float
ground_frequency(Float t)
{
  Float constexpr summit_time = 1.5;
  Float constexpr descent_time = 6;
  Float constexpr measure_start_time = 7;
  Float constexpr measure_end_time = 16;
  Float constexpr test_end_time = 18.5;

  Float constexpr start_freq = 0;
  Float constexpr summit_freq = 25;
  Float constexpr measure_start_freq = 18;
  Float constexpr measure_end_freq = 6;
  Float constexpr end_freq = 0;

  auto constexpr mk_slope_fn = [](Float t_start, Float t_end, Float f_start, Float f_end)
  {
    return [t_start, t_end, f_start, f_end](Float t)
    {
      return f_start + (f_end - f_start) / (t_end - t_start) * (t - t_start);
    };
  };

  auto constexpr initial_slope = mk_slope_fn(0, summit_time, start_freq, summit_freq);
  auto constexpr descent_slope = mk_slope_fn(descent_time, measure_start_time, summit_freq, measure_start_freq);
  auto constexpr measure_slope =
      mk_slope_fn(measure_start_time, measure_end_time, measure_start_freq, measure_end_freq);
  auto constexpr windown_slope = mk_slope_fn(measure_end_time, test_end_time, measure_end_freq, end_freq);

  if (t < summit_time)
  {
    return initial_slope(t);
  }
  if (t < descent_time)
  {
    return summit_freq;
  }
  if (t < measure_start_time)
  {
    return descent_slope(t);
  }
  if (t < measure_end_time)
  {
    return measure_slope(t);
  }
  if (t < test_end_time)
  {
    return windown_slope(t);
  }
  return end_freq;
}


template<std::floating_point Float>
static Float
ground_position(Float amplitude, Float phase)
{
  return amplitude * std::sin(phase);
}


template<std::floating_point Float>
auto
mk_state_derivate_fn(
    Float sprung_mass,
    Float unsprung_mass,
    Float suspension_spring_constant,
    Float suspension_damping_coefficient,
    Float tire_spring_constant,
    Float ground_amplitude)
{
  Float const ms = sprung_mass;
  Float const mu = unsprung_mass;
  Float const ks = suspension_spring_constant;
  Float const cs = suspension_damping_coefficient;
  Float const kt = tire_spring_constant;
  Float const ga = ground_amplitude;

  return [ms, mu, ks, cs, kt, ga](Float t, hilbertcli::state<Float> const &z) -> hilbertcli::derivative<Float>
  {
    Float const vphi = 2 * std::numbers::pi * ground_frequency(t);

    Float const vxs = z.vs();
    Float const vxu = z.vu();

    Float yg = ground_position(ga, z.phi());

    Float const vvs = (-cs * (z.vs() - z.vu()) - ks * (z.xs() - z.xu())) / ms;
    Float const vvu = (cs * (z.vs() - z.vu()) + ks * (z.xs() - z.xu()) - kt * (z.xu() - yg)) / mu;

    return hilbertcli::derivative{vphi, vxs, vxu, vvs, vvu};
  };
}


int
main()
{
  double constexpr sprung_mass = 270;                    // Sprung mass (kg)
  double constexpr unsprung_mass = 30;                   // Unsprung mass (kg)
  double constexpr suspension_spring_constant = 31000;   // Suspension spring constant (N/m)
  double constexpr suspension_damping_coefficient = 350; // Suspension damping coefficient (N·s/m)
  double constexpr tire_spring_constant = 196000;        // Tire spring constant (N/m)
  double constexpr time_step = 0.0005;                    // Simulation time step (s)
  double constexpr total_time = 20.0;                    // Total simulation time (s)
  double constexpr ground_amplitude = 0.003;             // Amplitude of platform motion (m)

  hilbertcli::state<double> state{0, 0, 0, 0, 0};

  auto const state_derivate_fn = mk_state_derivate_fn(
      sprung_mass,
      unsprung_mass,
      suspension_spring_constant,
      suspension_damping_coefficient,
      tire_spring_constant,
      ground_amplitude);

  auto const steps = static_cast<size_t>(total_time / time_step) + 1;

  std::vector<double> ground_data(steps);
  std::vector<double> tire_force_data(steps);

  for (size_t i = 0; i < steps; ++i)
  {
    auto const t = i * time_step;

    auto const ground = ground_position(ground_amplitude, state.phi());
    auto const ground_freq = ground_frequency(t);

    auto state_delta = rk4_delta(t, state, state_derivate_fn, time_step);
    state = state + state_delta;

    auto const tire_force = tire_spring_constant * (state.xu() - ground);

    ground_data[i] = ground;
    tire_force_data[i] = tire_force;

    //std::println("{} {} {} {} {} {}", t, state.xs(), state.xu(), ground_freq, ground, tire_force);
  }

  auto const sampling_rate = 1 / time_step;

  auto const ground_sd = hilbert::calculate_inst_signal_data(ground_data, sampling_rate);
  auto const tire_force_sd = hilbert::calculate_inst_signal_data(tire_force_data, sampling_rate);

  for (size_t i = 0; i < steps; ++i)
  {
    std::println(
        "{} {} {} {} {} {} {} {} {}",
        i * time_step,
        ground_data[i],
        tire_force_data[i],
        ground_sd.freq[i],
        tire_force_sd.freq[i],
        ground_sd.phase[i],
        tire_force_sd.phase[i],
        ground_sd.ampl[i],
        tire_force_sd.ampl[i]);
  }
}