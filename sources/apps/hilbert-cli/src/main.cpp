#include <chirp.hpp>
#include <collectors.hpp>
#include <derivative.hpp>
#include <print_utils.hpp>
#include <rk4.hpp>
#include <state.hpp>

#include <hilbert/hilbert.hpp>

#include <cmath>
#include <print>
#include <ranges>


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
constexpr auto
mk_ground_position_fn(Float amplitude)
{
  return [amplitude](Float phase)
  {
    return amplitude * std::sin(phase);
  };
}


template<std::floating_point Float>
constexpr auto
mk_state_derivate_fn(
    Float sprung_mass,
    Float unsprung_mass,
    Float suspension_spring_constant,
    Float suspension_damping_coefficient,
    Float tire_spring_constant,
    auto ground_position_fn)
{
  Float const ms = sprung_mass;
  Float const mu = unsprung_mass;
  Float const ks = suspension_spring_constant;
  Float const cs = suspension_damping_coefficient;
  Float const kt = tire_spring_constant;
  auto const ground_pos_fn = std::move(ground_position_fn);

  return
      [ms, mu, ks, cs, kt, ground_pos_fn](Float t, hilbertcli::state<Float> const &z) -> hilbertcli::derivative<Float>
  {
    Float const vphi = 2 * std::numbers::pi * ground_frequency(t);

    Float const vxs = z.vs();
    Float const vxu = z.vu();

    Float yg = ground_pos_fn(z.phi());

    Float const vvs = (-cs * (z.vs() - z.vu()) - ks * (z.xs() - z.xu())) / ms;
    Float const vvu = (cs * (z.vs() - z.vu()) + ks * (z.xs() - z.xu()) - kt * (z.xu() - yg)) / mu;

    return hilbertcli::derivative{vphi, vxs, vxu, vvs, vvu};
  };
}


void
print_values(std::ranges::contiguous_range auto... ranges)
{
  auto r = std::views::zip(ranges...);
  std::ranges::for_each(std::move(r), [](auto const &elem) {
    std::apply([](auto const &...e) { hilbertcli::println_join<' '>(e...); }, elem);
  });
}


int
main()
{
  double constexpr sprung_mass = 270;                    // Sprung mass (kg)
  double constexpr unsprung_mass = 30;                   // Unsprung mass (kg)
  double constexpr suspension_spring_constant = 31000;   // Suspension spring constant (N/m)
  double constexpr suspension_damping_coefficient = 350; // Suspension damping coefficient (N·s/m)
  double constexpr tire_spring_constant = 196000;        // Tire spring constant (N/m)
  double constexpr time_step = 0.0005;                   // Simulation time step (s)
  double constexpr total_time = 20.0;                    // Total simulation time (s)
  double constexpr ground_amplitude = 0.003;             // Amplitude of platform motion (m)

  hilbertcli::state<double> state{0, 0, 0, 0, 0};

  auto constexpr ground_positon_fn = mk_ground_position_fn(ground_amplitude);

  auto constexpr state_derivate_fn = mk_state_derivate_fn(
      sprung_mass,
      unsprung_mass,
      suspension_spring_constant,
      suspension_damping_coefficient,
      tire_spring_constant,
      ground_positon_fn);

  auto constexpr steps = static_cast<size_t>(total_time / time_step);

  auto const initial_ground_data = ground_positon_fn(state.phi());

  hilbertcli::vec_collector collector(
      steps + 1,
      {0, state.xs(), state.xu(), initial_ground_data, tire_spring_constant * (state.xu() - initial_ground_data)});

  for (size_t i = 0; i < steps; ++i)
  {
    auto const t = i * time_step;

    auto const ground = ground_positon_fn(state.phi());

    auto const state_delta = rk4_delta(t, state, state_derivate_fn, time_step);
    state = state + state_delta;

    auto const tire_force = tire_spring_constant * (state.xu() - ground);

    collector.collect({t + time_step, state.xs(), state.xu(), ground, tire_force});
  }

  auto constexpr sampling_rate = 1 / time_step;

  auto ground_data = collector.ground_span();
  auto tire_force_data = collector.tire_force_span();

  auto const ground_sd = hilbert::calculate_inst_signal_data(ground_data, sampling_rate);
  auto const tire_force_sd = hilbert::calculate_inst_signal_data(tire_force_data, sampling_rate);

  print_values(
      collector.time_span(),
      collector.xs_span(),
      collector.xu_span(),
      ground_data,
      tire_force_data,
      ground_sd.ampl,
      ground_sd.phase,
      ground_sd.freq,
      tire_force_sd.ampl,
      tire_force_sd.phase,
      tire_force_sd.freq);
}
