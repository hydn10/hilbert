#ifndef HILBERT_SIMULATION_SINKS_NORMAL_EQUATIONS_HPP
#define HILBERT_SIMULATION_SINKS_NORMAL_EQUATIONS_HPP


#include <hilbert/analysis/least_squares/normal_equations3.hpp>
#include <hilbert/analysis/least_squares/observation.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/detail/attributes.hpp>
#include <hilbert/simulation/core/input_count.hpp>

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>


namespace hilbert::simulation::sinks
{

template<typename Reducer, typename ObservationFactory>
class normal_equations_sink
{
  HILBERT_NO_UNIQUE_ADDRESS Reducer reducer_;
  HILBERT_NO_UNIQUE_ADDRESS ObservationFactory observation_factory_;

public:
  normal_equations_sink(Reducer reducer, ObservationFactory observation_factory);

  template<typename Sample>
  requires std::same_as<
      std::remove_cvref_t<std::invoke_result_t<ObservationFactory const &, Sample>>,
      hilbert::analysis::least_squares_observation<typename Reducer::float_type, Reducer::response_count>>
  void
  push(Sample sample);

  [[nodiscard]]
  auto
  finish() &&;
};


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
class normal_equations_sink_factory
{
  HILBERT_NO_UNIQUE_ADDRESS Basis basis_;
  HILBERT_NO_UNIQUE_ADDRESS ObservationFactory observation_factory_;

  [[nodiscard]]
  auto
  make_reducer(exact_input_count count) const;

  [[nodiscard]]
  auto
  make_reducer([[maybe_unused]] input_count_upper_bound count) const;

  [[nodiscard]]
  auto
  make_reducer([[maybe_unused]] unknown_input_count count) const;

public:
  normal_equations_sink_factory(Basis basis, ObservationFactory observation_factory);

  template<input_count_descriptor Count>
  [[nodiscard]]
  auto
  operator()(Count count) const;
};


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
[[nodiscard]]
auto
make_normal_equations_sink_factory(Basis basis, ObservationFactory observation_factory);


template<typename Reducer, typename ObservationFactory>
normal_equations_sink<Reducer, ObservationFactory>::normal_equations_sink(
    Reducer reducer, ObservationFactory observation_factory)
    : reducer_{std::move(reducer)}
    , observation_factory_{std::move(observation_factory)}
{
}


template<typename Reducer, typename ObservationFactory>
template<typename Sample>
requires std::same_as<
    std::remove_cvref_t<std::invoke_result_t<ObservationFactory const &, Sample>>,
    hilbert::analysis::least_squares_observation<typename Reducer::float_type, Reducer::response_count>>
void
normal_equations_sink<Reducer, ObservationFactory>::push(Sample sample)
{
  reducer_.accumulate(std::invoke(observation_factory_, std::move(sample)));
}


template<typename Reducer, typename ObservationFactory>
auto
normal_equations_sink<Reducer, ObservationFactory>::finish() &&
{
  return std::move(reducer_).finish();
}


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory>::make_reducer(
    exact_input_count count) const
{
  return hilbert::analysis::make_normal_equations_reducer<Float, ResponseCount>(
      basis_, hilbert::analysis::exact_observation_count{count.value()});
}


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory>::make_reducer(
    [[maybe_unused]] input_count_upper_bound count) const
{
  return hilbert::analysis::make_normal_equations_reducer<Float, ResponseCount>(
      basis_, hilbert::analysis::count_observations);
}


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory>::make_reducer(
    [[maybe_unused]] unknown_input_count count) const
{
  return hilbert::analysis::make_normal_equations_reducer<Float, ResponseCount>(
      basis_, hilbert::analysis::count_observations);
}


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory>::normal_equations_sink_factory(
    Basis basis, ObservationFactory observation_factory)
    : basis_{std::move(basis)}
    , observation_factory_{std::move(observation_factory)}
{
}


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
template<input_count_descriptor Count>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory>::operator()(Count count) const
{
  return normal_equations_sink{make_reducer(count), observation_factory_};
}


template<supported_float Float, std::size_t ResponseCount, typename Basis, typename ObservationFactory>
auto
make_normal_equations_sink_factory(Basis basis, ObservationFactory observation_factory)
{
  return normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory>{
      std::move(basis),
      std::move(observation_factory),
  };
}

} // namespace hilbert::simulation::sinks

#endif // HILBERT_SIMULATION_SINKS_NORMAL_EQUATIONS_HPP
