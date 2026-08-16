#ifndef HILBERT_SIMULATION_SUSPENSION_SINKS_SOA_VECTOR_HPP
#define HILBERT_SIMULATION_SUSPENSION_SINKS_SOA_VECTOR_HPP


#include <hilbert/simulation/suspension/sample.hpp>

#include <concepts>
#include <cstddef>
#include <span>
#include <utility>
#include <vector>


namespace hilbert::simulation::suspension::sinks
{

template<std::floating_point Float>
class soa_vector_sink
{
public:
  class simulation_data;

private:
  std::vector<Float> time_data_;
  std::vector<Float> sprung_displacement_data_;
  std::vector<Float> unsprung_displacement_data_;
  std::vector<Float> ground_displacement_data_;
  std::vector<Float> tire_force_data_;

public:
  explicit soa_vector_sink(size_t sample_count);

  soa_vector_sink(soa_vector_sink const &) = delete;
  soa_vector_sink &
  operator=(soa_vector_sink const &) = delete;
  soa_vector_sink(soa_vector_sink &&) noexcept;
  soa_vector_sink &
  operator=(soa_vector_sink &&) noexcept;

  void
  push(sample<Float> data_point);

  simulation_data
  finish() &&;
};


template<std::floating_point Float>
class soa_vector_sink<Float>::simulation_data
{
  std::vector<Float> time_data_;
  std::vector<Float> sprung_displacement_data_;
  std::vector<Float> unsprung_displacement_data_;
  std::vector<Float> ground_displacement_data_;
  std::vector<Float> tire_force_data_;

  simulation_data(
      std::vector<Float> &&time_data,
      std::vector<Float> &&sprung_displacement_data,
      std::vector<Float> &&unsprung_displacement_data,
      std::vector<Float> &&ground_displacement_data,
      std::vector<Float> &&tire_force_data);

  friend class soa_vector_sink<Float>;

public:
  simulation_data(simulation_data const &) = delete;
  simulation_data &
  operator=(simulation_data const &) = delete;
  simulation_data(simulation_data &&) noexcept = default;
  simulation_data &
  operator=(simulation_data &&) noexcept = default;

  std::span<Float const>
  time_span() const;

  std::span<Float const>
  sprung_displacement_span() const;

  std::span<Float const>
  unsprung_displacement_span() const;

  std::span<Float const>
  ground_displacement_span() const;

  std::span<Float const>
  tire_force_span() const;
};


template<std::floating_point Float>
struct soa_vector_sink_factory
{
  soa_vector_sink<Float>
  operator()(size_t sample_count) const;
};


template<std::floating_point Float>
soa_vector_sink<Float>::simulation_data::simulation_data(
    std::vector<Float> &&time_data,
    std::vector<Float> &&sprung_displacement_data,
    std::vector<Float> &&unsprung_displacement_data,
    std::vector<Float> &&ground_displacement_data,
    std::vector<Float> &&tire_force_data)
    : time_data_{std::move(time_data)}
    , sprung_displacement_data_{std::move(sprung_displacement_data)}
    , unsprung_displacement_data_{std::move(unsprung_displacement_data)}
    , ground_displacement_data_{std::move(ground_displacement_data)}
    , tire_force_data_{std::move(tire_force_data)}
{
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::time_span() const
{
  return time_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::sprung_displacement_span() const
{
  return sprung_displacement_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::unsprung_displacement_span() const
{
  return unsprung_displacement_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::ground_displacement_span() const
{
  return ground_displacement_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::tire_force_span() const
{
  return tire_force_data_;
}


template<std::floating_point Float>
soa_vector_sink<Float>::soa_vector_sink(size_t sample_count)
{
  time_data_.reserve(sample_count);
  sprung_displacement_data_.reserve(sample_count);
  unsprung_displacement_data_.reserve(sample_count);
  ground_displacement_data_.reserve(sample_count);
  tire_force_data_.reserve(sample_count);
}


template<std::floating_point Float>
soa_vector_sink<Float>::soa_vector_sink(soa_vector_sink &&) noexcept = default;


template<std::floating_point Float>
soa_vector_sink<Float> &
soa_vector_sink<Float>::operator=(soa_vector_sink &&) noexcept = default;


template<std::floating_point Float>
void
soa_vector_sink<Float>::push(sample<Float> data_point)
{
  time_data_.emplace_back(data_point.time);
  sprung_displacement_data_.emplace_back(data_point.sprung_displacement);
  unsprung_displacement_data_.emplace_back(data_point.unsprung_displacement);
  ground_displacement_data_.emplace_back(data_point.ground_displacement);
  tire_force_data_.emplace_back(data_point.tire_force);
}


template<std::floating_point Float>
typename soa_vector_sink<Float>::simulation_data
soa_vector_sink<Float>::finish() &&
{
  return simulation_data{
      std::move(time_data_),
      std::move(sprung_displacement_data_),
      std::move(unsprung_displacement_data_),
      std::move(ground_displacement_data_),
      std::move(tire_force_data_),
  };
}


template<std::floating_point Float>
soa_vector_sink<Float>
soa_vector_sink_factory<Float>::operator()(size_t sample_count) const
{
  return soa_vector_sink<Float>{sample_count};
}

} // namespace hilbert::simulation::suspension::sinks

#endif // HILBERT_SIMULATION_SUSPENSION_SINKS_SOA_VECTOR_HPP
