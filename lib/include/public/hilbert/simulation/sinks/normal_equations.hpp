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

template<typename Sample, typename ObservationFactory, typename Float, std::size_t ResponseCount>
concept observation_factory_for = requires(ObservationFactory &factory, Sample sample) {
  requires std::same_as<
      std::remove_cvref_t<decltype(std::invoke(factory, std::move(sample)))>,
      hilbert::analysis::least_squares_observation<Float, ResponseCount>>;
};


template<typename Reducer, typename ObservationFactory>
class normal_equations_sink
{
  HILBERT_NO_UNIQUE_ADDRESS Reducer reducer_;
  HILBERT_NO_UNIQUE_ADDRESS ObservationFactory observation_factory_;

public:
  normal_equations_sink(Reducer reducer, ObservationFactory observation_factory);

  template<observation_factory_for<ObservationFactory, typename Reducer::float_type, Reducer::response_count> Sample>
  void
  push(Sample sample);

  [[nodiscard]]
  auto
  finish() &&;
};


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector =
        hilbert::analysis::no_least_squares_statistics_collector<Float, ResponseCount>>
class normal_equations_sink_factory
{
  HILBERT_NO_UNIQUE_ADDRESS Basis basis_;
  HILBERT_NO_UNIQUE_ADDRESS ObservationFactory observation_factory_;
  HILBERT_NO_UNIQUE_ADDRESS StatisticsCollector statistics_collector_;

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
  normal_equations_sink_factory(
      Basis basis, ObservationFactory observation_factory, StatisticsCollector statistics_collector);

  template<input_count_descriptor Count>
  [[nodiscard]]
  auto
  operator()(Count count) const;
};


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector =
        hilbert::analysis::no_least_squares_statistics_collector<Float, ResponseCount>>
[[nodiscard]]
auto
make_normal_equations_sink_factory(
    Basis basis, ObservationFactory observation_factory, StatisticsCollector statistics_collector = {});


template<typename Reducer, typename ObservationFactory>
normal_equations_sink<Reducer, ObservationFactory>::normal_equations_sink(
    Reducer reducer, ObservationFactory observation_factory)
    : reducer_{std::move(reducer)}
    , observation_factory_{std::move(observation_factory)}
{
}


template<typename Reducer, typename ObservationFactory>
template<observation_factory_for<ObservationFactory, typename Reducer::float_type, Reducer::response_count> Sample>
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


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory, StatisticsCollector>::make_reducer(
    exact_input_count count) const
{
  return hilbert::analysis::make_normal_equations_reducer<Float, ResponseCount>(
      basis_, hilbert::analysis::exact_observation_count{count.value()}, statistics_collector_);
}


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory, StatisticsCollector>::make_reducer(
    [[maybe_unused]] input_count_upper_bound count) const
{
  return hilbert::analysis::make_normal_equations_reducer<Float, ResponseCount>(
      basis_, hilbert::analysis::count_observations, statistics_collector_);
}


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory, StatisticsCollector>::make_reducer(
    [[maybe_unused]] unknown_input_count count) const
{
  return hilbert::analysis::make_normal_equations_reducer<Float, ResponseCount>(
      basis_, hilbert::analysis::count_observations, statistics_collector_);
}


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector>
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory, StatisticsCollector>::
    normal_equations_sink_factory(
        Basis basis, ObservationFactory observation_factory, StatisticsCollector statistics_collector)
    : basis_{std::move(basis)}
    , observation_factory_{std::move(observation_factory)}
    , statistics_collector_{std::move(statistics_collector)}
{
}


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector>
template<input_count_descriptor Count>
auto
normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory, StatisticsCollector>::operator()(
    Count count) const
{
  return normal_equations_sink{make_reducer(count), observation_factory_};
}


template<
    supported_float Float,
    std::size_t ResponseCount,
    hilbert::analysis::basis_for_size<3uz, Float> Basis,
    typename ObservationFactory,
    hilbert::analysis::least_squares_statistics_collector_for<Float, ResponseCount> StatisticsCollector>
auto
make_normal_equations_sink_factory(
    Basis basis, ObservationFactory observation_factory, StatisticsCollector statistics_collector)
{
  return normal_equations_sink_factory<Float, ResponseCount, Basis, ObservationFactory, StatisticsCollector>{
      std::move(basis),
      std::move(observation_factory),
      std::move(statistics_collector),
  };
}

} // namespace hilbert::simulation::sinks

#endif // HILBERT_SIMULATION_SINKS_NORMAL_EQUATIONS_HPP
