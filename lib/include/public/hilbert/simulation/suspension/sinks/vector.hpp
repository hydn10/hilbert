#ifndef HILBERT_SIMULATION_SUSPENSION_SINKS_VECTOR_HPP
#define HILBERT_SIMULATION_SUSPENSION_SINKS_VECTOR_HPP


#include <hilbert/simulation/suspension/sample.hpp>

#include <concepts>
#include <cstddef>
#include <span>
#include <utility>
#include <vector>


namespace hilbert::simulation::suspension::sinks
{

template<std::floating_point Float>
class vector_sink
{
  std::vector<sample<Float>> samples_;

public:
  class simulation_data;

  explicit vector_sink(size_t sample_count);

  vector_sink(vector_sink const &) = delete;
  vector_sink &
  operator=(vector_sink const &) = delete;
  vector_sink(vector_sink &&) noexcept;
  vector_sink &
  operator=(vector_sink &&) noexcept;
  ~vector_sink() = default;

  void
  push(sample<Float> data_point);

  simulation_data
  finish() &&;
};


template<std::floating_point Float>
class vector_sink<Float>::simulation_data
{
  std::vector<sample<Float>> samples_;

  explicit simulation_data(std::vector<sample<Float>> &&samples);

  friend class vector_sink<Float>;

public:
  simulation_data(simulation_data const &) = delete;
  simulation_data &
  operator=(simulation_data const &) = delete;
  simulation_data(simulation_data &&) noexcept = default;
  simulation_data &
  operator=(simulation_data &&) noexcept = default;
  ~simulation_data() = default;

  std::span<sample<Float> const>
  samples() const;
};


template<std::floating_point Float>
struct vector_sink_factory
{
  vector_sink<Float>
  operator()(size_t sample_count) const;
};


template<std::floating_point Float>
vector_sink<Float>::simulation_data::simulation_data(std::vector<sample<Float>> &&samples)
    : samples_{std::move(samples)}
{
}


template<std::floating_point Float>
std::span<sample<Float> const>
vector_sink<Float>::simulation_data::samples() const
{
  return samples_;
}


template<std::floating_point Float>
vector_sink<Float>::vector_sink(size_t sample_count)
{
  samples_.reserve(sample_count);
}


template<std::floating_point Float>
vector_sink<Float>::vector_sink(vector_sink &&) noexcept = default;


template<std::floating_point Float>
vector_sink<Float> &
vector_sink<Float>::operator=(vector_sink &&) noexcept = default;


template<std::floating_point Float>
void
vector_sink<Float>::push(sample<Float> data_point)
{
  samples_.emplace_back(data_point);
}


template<std::floating_point Float>
vector_sink<Float>::simulation_data
vector_sink<Float>::finish() &&
{
  return simulation_data{std::move(samples_)};
}


template<std::floating_point Float>
vector_sink<Float>
vector_sink_factory<Float>::operator()(size_t sample_count) const
{
  return vector_sink<Float>{sample_count};
}

} // namespace hilbert::simulation::suspension::sinks

#endif // HILBERT_SIMULATION_SUSPENSION_SINKS_VECTOR_HPP
