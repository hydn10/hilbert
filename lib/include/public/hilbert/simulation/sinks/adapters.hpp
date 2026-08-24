#ifndef HILBERT_SIMULATION_SINKS_ADAPTERS_HPP
#define HILBERT_SIMULATION_SINKS_ADAPTERS_HPP


#include <hilbert/detail/attributes.hpp>
#include <hilbert/simulation/core/input_count.hpp>

#include <cstddef>
#include <functional>
#include <tuple>
#include <utility>


namespace hilbert::simulation::sinks
{

template<typename Sink, typename Predicate>
class filtered_sink
{
  HILBERT_NO_UNIQUE_ADDRESS Sink sink_;
  HILBERT_NO_UNIQUE_ADDRESS Predicate predicate_;

public:
  filtered_sink(Sink sink, Predicate predicate);

  template<typename Sample>
  void
  push(Sample sample);

  [[nodiscard]]
  auto
  finish() &&;
};


template<typename DownstreamFactory, typename Predicate>
class filtered_sink_factory
{
  HILBERT_NO_UNIQUE_ADDRESS DownstreamFactory downstream_factory_;
  HILBERT_NO_UNIQUE_ADDRESS Predicate predicate_;

public:
  filtered_sink_factory(DownstreamFactory downstream_factory, Predicate predicate);

  template<input_count_descriptor Count>
  [[nodiscard]]
  auto
  operator()(Count count) const;
};


template<typename FirstSink, typename SecondSink>
class tee_sink
{
  FirstSink first_;
  SecondSink second_;

public:
  tee_sink(FirstSink first, SecondSink second);

  template<typename Sample>
  void
  push(Sample sample);

  [[nodiscard]]
  auto
  finish() &&;
};


template<typename FirstFactory, typename SecondFactory>
class tee_sink_factory
{
  HILBERT_NO_UNIQUE_ADDRESS FirstFactory first_factory_;
  HILBERT_NO_UNIQUE_ADDRESS SecondFactory second_factory_;

public:
  tee_sink_factory(FirstFactory first_factory, SecondFactory second_factory);

  template<input_count_descriptor Count>
  [[nodiscard]]
  auto
  operator()(Count count) const;
};


template<typename DownstreamFactory, typename Predicate>
[[nodiscard]]
auto
make_filtered_sink_factory(DownstreamFactory downstream_factory, Predicate predicate);


template<typename FirstFactory, typename SecondFactory>
[[nodiscard]]
auto
make_tee_sink_factory(FirstFactory first_factory, SecondFactory second_factory);


template<typename Sink, typename Predicate>
filtered_sink<Sink, Predicate>::filtered_sink(Sink sink, Predicate predicate)
    : sink_{std::move(sink)}
    , predicate_{std::move(predicate)}
{
}


template<typename Sink, typename Predicate>
template<typename Sample>
void
filtered_sink<Sink, Predicate>::push(Sample sample)
{
  if (std::invoke(predicate_, sample))
  {
    sink_.push(std::move(sample));
  }
}


template<typename Sink, typename Predicate>
auto
filtered_sink<Sink, Predicate>::finish() &&
{
  return std::move(sink_).finish();
}


template<typename DownstreamFactory, typename Predicate>
filtered_sink_factory<DownstreamFactory, Predicate>::filtered_sink_factory(
    DownstreamFactory downstream_factory, Predicate predicate)
    : downstream_factory_{std::move(downstream_factory)}
    , predicate_{std::move(predicate)}
{
}


template<typename DownstreamFactory, typename Predicate>
template<input_count_descriptor Count>
auto
filtered_sink_factory<DownstreamFactory, Predicate>::operator()(Count count) const
{
  return filtered_sink{
      std::invoke(downstream_factory_, as_upper_bound(count)),
      predicate_,
  };
}


template<typename FirstSink, typename SecondSink>
tee_sink<FirstSink, SecondSink>::tee_sink(FirstSink first, SecondSink second)
    : first_{std::move(first)}
    , second_{std::move(second)}
{
}


template<typename FirstSink, typename SecondSink>
template<typename Sample>
void
tee_sink<FirstSink, SecondSink>::push(Sample sample)
{
  first_.push(sample);
  second_.push(std::move(sample));
}


template<typename FirstSink, typename SecondSink>
auto
tee_sink<FirstSink, SecondSink>::finish() &&
{
  return std::tuple{
      std::move(first_).finish(),
      std::move(second_).finish(),
  };
}


template<typename FirstFactory, typename SecondFactory>
tee_sink_factory<FirstFactory, SecondFactory>::tee_sink_factory(
    FirstFactory first_factory, SecondFactory second_factory)
    : first_factory_{std::move(first_factory)}
    , second_factory_{std::move(second_factory)}
{
}


template<typename FirstFactory, typename SecondFactory>
template<input_count_descriptor Count>
auto
tee_sink_factory<FirstFactory, SecondFactory>::operator()(Count count) const
{
  return tee_sink{
      std::invoke(first_factory_, count),
      std::invoke(second_factory_, count),
  };
}


template<typename DownstreamFactory, typename Predicate>
auto
make_filtered_sink_factory(DownstreamFactory downstream_factory, Predicate predicate)
{
  return filtered_sink_factory<DownstreamFactory, Predicate>{
      std::move(downstream_factory),
      std::move(predicate),
  };
}


template<typename FirstFactory, typename SecondFactory>
auto
make_tee_sink_factory(FirstFactory first_factory, SecondFactory second_factory)
{
  return tee_sink_factory<FirstFactory, SecondFactory>{
      std::move(first_factory),
      std::move(second_factory),
  };
}

} // namespace hilbert::simulation::sinks

#endif // HILBERT_SIMULATION_SINKS_ADAPTERS_HPP
