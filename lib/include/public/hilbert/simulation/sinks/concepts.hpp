#ifndef HILBERT_SIMULATION_SINKS_CONCEPTS_HPP
#define HILBERT_SIMULATION_SINKS_CONCEPTS_HPP


#include <hilbert/simulation/core/input_count.hpp>

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>


namespace hilbert::simulation::sinks
{

template<typename Sink, typename Sample>
concept sink_push_for = requires(Sink &sink, Sample sample) {
  { sink.push(std::move(sample)) } -> std::same_as<void>;
};


template<typename Sink>
concept sink_finishable = requires(Sink &sink) { std::move(sink).finish(); };


template<typename Sink, typename Sample>
concept sink_for = sink_push_for<Sink, Sample> && sink_finishable<Sink>;


template<typename SinkFactory, typename Count, typename Sample>
concept sink_factory_for = input_count_descriptor<Count> && requires(SinkFactory &&factory, Count const &count) {
  requires sink_for<std::remove_cvref_t<decltype(std::invoke(std::forward<SinkFactory>(factory), count))>, Sample>;
};

} // namespace hilbert::simulation::sinks

#endif // HILBERT_SIMULATION_SINKS_CONCEPTS_HPP
