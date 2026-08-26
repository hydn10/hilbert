#include <hilbert/simulation/core/concepts.hpp>
#include <hilbert/simulation/core/input_count.hpp>
#include <hilbert/simulation/core/problem.hpp>
#include <hilbert/simulation/core/settings.hpp>
#include <hilbert/simulation/drivers/simulation_view.hpp>
#include <hilbert/simulation/integrators/rk4.hpp>
#include <hilbert/simulation/sinks/concepts.hpp>
#include <hilbert/simulation/suspension/ground_frequencies/constant.hpp>
#include <hilbert/simulation/suspension/model.hpp>
#include <hilbert/simulation/suspension/state.hpp>

#include <concepts>
#include <ranges>


namespace
{

class test_delta
{
  double value_;

public:
  explicit test_delta(double value = 0)
      : value_{value}
  {
  }

  [[nodiscard]]
  double
  value() const
  {
    return value_;
  }

  [[nodiscard]]
  test_delta
  operator+(test_delta const &other) const
  {
    return test_delta{value_ + other.value_};
  }

  [[nodiscard]]
  test_delta
  operator*(double scalar) const
  {
    return test_delta{value_ * scalar};
  }
};


class test_state
{
  double value_;

public:
  explicit test_state(double value = 0)
      : value_{value}
  {
  }

  [[nodiscard]]
  double
  value() const
  {
    return value_;
  }
};


class non_assignable_state
{
  double value_;

public:
  explicit non_assignable_state(double value = 0)
      : value_{value}
  {
  }

  non_assignable_state(non_assignable_state const &) = default;
  non_assignable_state(non_assignable_state &&) = default;
  non_assignable_state &
  operator=(non_assignable_state const &) = delete;
  non_assignable_state &
  operator=(non_assignable_state &&) = delete;
  ~non_assignable_state() = default;

  [[nodiscard]]
  double
  value() const
  {
    return value_;
  }
};


[[nodiscard]]
test_state
operator+(test_state const &state, test_delta const &delta)
{
  return test_state{state.value() + delta.value()};
}


[[nodiscard]]
non_assignable_state
operator+(non_assignable_state const &state, test_delta const &delta)
{
  return non_assignable_state{state.value() + delta.value()};
}


struct test_sample
{
};


struct test_sink
{
  void push(test_sample);

  int
  finish() &&;
};


struct invalid_sink
{
  void push(test_sample);
};


struct test_sink_factory
{
  test_sink
  operator()(hilbert::simulation::exact_input_count) const;
};


struct invalid_sink_factory
{
  int
  operator()(hilbert::simulation::exact_input_count) const;
};


struct test_dynamics
{
  [[nodiscard]]
  static test_delta
  derivative([[maybe_unused]] double time, test_state const &state)
  {
    return test_delta{state.value()};
  }
};


struct reference_dynamics
{
  [[nodiscard]]
  static test_delta &
  derivative([[maybe_unused]] double time, [[maybe_unused]] test_state const &state)
  {
    static test_delta result{};
    return result;
  }
};


struct test_model
{
  [[nodiscard]]
  static test_delta
  derivative([[maybe_unused]] double time, test_state const &state)
  {
    return test_delta{state.value()};
  }

  [[nodiscard]]
  static test_sample
  observe([[maybe_unused]] double time, test_state const &state)
  {
    static_cast<void>(state);
    return {};
  }
};


struct non_assignable_model
{
  [[nodiscard]]
  static test_delta
  derivative([[maybe_unused]] double time, non_assignable_state const &state)
  {
    return test_delta{state.value()};
  }

  [[nodiscard]]
  static test_sample
  observe([[maybe_unused]] double time, non_assignable_state const &state)
  {
    static_cast<void>(state);
    return {};
  }
};


struct reference_model
{
  [[nodiscard]]
  static test_delta
  derivative([[maybe_unused]] double time, test_state const &state)
  {
    return test_delta{state.value()};
  }

  [[nodiscard]]
  static test_sample &
  observe([[maybe_unused]] double time, [[maybe_unused]] test_state const &state)
  {
    static test_sample result{};
    return result;
  }
};


struct stateful_integrator
{
  [[nodiscard]]
  test_delta
  operator()(
      [[maybe_unused]] double time,
      test_state const &state,
      [[maybe_unused]] test_model const &model,
      [[maybe_unused]] double time_step)
  {
    return test_delta{state.value()};
  }
};


struct non_assignable_integrator
{
  [[nodiscard]]
  test_delta
  operator()(
      [[maybe_unused]] double time,
      non_assignable_state const &state,
      [[maybe_unused]] non_assignable_model const &model,
      [[maybe_unused]] double time_step)
  {
    return test_delta{state.value()};
  }
};


template<typename State, typename Model, typename Integrator>
concept simulation_problem_type =
    requires { typename hilbert::simulation::simulation_problem<double, State, Model, Integrator>; };


using rk4_integrator = hilbert::simulation::integrators::rk4<double, test_state, test_model>;
using test_problem = hilbert::simulation::simulation_problem<double, test_state, test_model, stateful_integrator>;
using tested_view = hilbert::simulation::simulation_view<test_problem>;
using suspension_model = hilbert::simulation::suspension::
    model<double, hilbert::simulation::suspension::ground_frequencies::constant<double>>;

static_assert(hilbert::simulation::state_delta_algebra<test_state, test_delta, double>);
static_assert(hilbert::simulation::state_delta_algebra<non_assignable_state, test_delta, double>);
static_assert(hilbert::simulation::executable_state<test_state>);
static_assert(!hilbert::simulation::executable_state<non_assignable_state>);
static_assert(hilbert::simulation::dynamics_for<test_dynamics, double, test_state>);
static_assert(!hilbert::simulation::dynamics_for<reference_dynamics, double, test_state>);
static_assert(hilbert::simulation::physical_model_for<test_model, double, test_state>);
static_assert(hilbert::simulation::physical_model_for<non_assignable_model, double, non_assignable_state>);
static_assert(!hilbert::simulation::physical_model_for<reference_model, double, test_state>);
static_assert(hilbert::simulation::integrator_for<rk4_integrator, double, test_state, test_model>);
static_assert(hilbert::simulation::integrator_for<stateful_integrator, double, test_state, test_model>);
static_assert(
    hilbert::simulation::integrator_for<non_assignable_integrator, double, non_assignable_state, non_assignable_model>);
static_assert(hilbert::simulation::executable_state<hilbert::simulation::suspension::state<double>>);
static_assert(hilbert::simulation::state_delta_algebra<
              hilbert::simulation::suspension::state<double>,
              hilbert::simulation::suspension::state_derivative<double>,
              double>);
static_assert(
    hilbert::simulation::dynamics_for<suspension_model, double, hilbert::simulation::suspension::state<double>>);
static_assert(
    hilbert::simulation::physical_model_for<suspension_model, double, hilbert::simulation::suspension::state<double>>);
static_assert(hilbert::simulation::simulation_problem_for<test_problem>);
static_assert(hilbert::simulation::sinks::sink_for<test_sink, test_sample>);
static_assert(!hilbert::simulation::sinks::sink_for<invalid_sink, test_sample>);
static_assert(hilbert::simulation::sinks::
                  sink_factory_for<test_sink_factory, hilbert::simulation::exact_input_count, test_sample>);
static_assert(!hilbert::simulation::sinks::
                  sink_factory_for<invalid_sink_factory, hilbert::simulation::exact_input_count, test_sample>);
static_assert(!simulation_problem_type<non_assignable_state, non_assignable_model, non_assignable_integrator>);
static_assert(std::constructible_from<
              test_problem,
              hilbert::simulation::simulation_settings<double>,
              test_state,
              test_model,
              stateful_integrator>);

static_assert(std::ranges::input_range<tested_view>);
static_assert(std::ranges::sized_range<tested_view>);
static_assert(std::ranges::view<tested_view>);
static_assert(!std::ranges::forward_range<tested_view>);

} // namespace
