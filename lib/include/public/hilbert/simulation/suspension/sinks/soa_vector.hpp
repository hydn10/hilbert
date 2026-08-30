#ifndef HILBERT_SIMULATION_SUSPENSION_SINKS_SOA_VECTOR_HPP
#define HILBERT_SIMULATION_SUSPENSION_SINKS_SOA_VECTOR_HPP


#include <hilbert/simulation/core/input_count.hpp>
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
  std::vector<Float> time_data_;
  std::vector<Float> sprung_displacement_data_;
  std::vector<Float> unsprung_displacement_data_;
  std::vector<Float> ground_displacement_data_;
  std::vector<Float> tire_force_data_;

public:
  class simulation_data;

  explicit soa_vector_sink(size_t sample_count);

  soa_vector_sink(soa_vector_sink const &) = delete;
  soa_vector_sink &
  operator=(soa_vector_sink const &) = delete;
  soa_vector_sink(soa_vector_sink &&) noexcept;
  soa_vector_sink &
  operator=(soa_vector_sink &&) noexcept;
  ~soa_vector_sink() = default;

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
  ~simulation_data() = default;

  [[nodiscard]]
  std::span<Float>
  time_span() noexcept;

  [[nodiscard]]
  std::span<Float const>
  time_span() const noexcept;

  [[nodiscard]]
  std::span<Float>
  sprung_displacement_span() noexcept;

  [[nodiscard]]
  std::span<Float const>
  sprung_displacement_span() const noexcept;

  [[nodiscard]]
  std::span<Float>
  unsprung_displacement_span() noexcept;

  [[nodiscard]]
  std::span<Float const>
  unsprung_displacement_span() const noexcept;

  [[nodiscard]]
  std::span<Float>
  ground_displacement_span() noexcept;

  [[nodiscard]]
  std::span<Float const>
  ground_displacement_span() const noexcept;

  [[nodiscard]]
  std::span<Float>
  tire_force_span() noexcept;

  [[nodiscard]]
  std::span<Float const>
  tire_force_span() const noexcept;
};


template<std::floating_point Float>
struct soa_vector_sink_factory
{
  template<hilbert::simulation::input_count_descriptor Count>
  soa_vector_sink<Float>
  operator()(Count count) const;
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
std::span<Float>
soa_vector_sink<Float>::simulation_data::time_span() noexcept
{
  return time_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::time_span() const noexcept
{
  return time_data_;
}


template<std::floating_point Float>
std::span<Float>
soa_vector_sink<Float>::simulation_data::sprung_displacement_span() noexcept
{
  return sprung_displacement_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::sprung_displacement_span() const noexcept
{
  return sprung_displacement_data_;
}


template<std::floating_point Float>
std::span<Float>
soa_vector_sink<Float>::simulation_data::unsprung_displacement_span() noexcept
{
  return unsprung_displacement_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::unsprung_displacement_span() const noexcept
{
  return unsprung_displacement_data_;
}


template<std::floating_point Float>
std::span<Float>
soa_vector_sink<Float>::simulation_data::ground_displacement_span() noexcept
{
  return ground_displacement_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::ground_displacement_span() const noexcept
{
  return ground_displacement_data_;
}


template<std::floating_point Float>
std::span<Float>
soa_vector_sink<Float>::simulation_data::tire_force_span() noexcept
{
  return tire_force_data_;
}


template<std::floating_point Float>
std::span<Float const>
soa_vector_sink<Float>::simulation_data::tire_force_span() const noexcept
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
soa_vector_sink<Float>::simulation_data
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
template<hilbert::simulation::input_count_descriptor Count>
soa_vector_sink<Float>
soa_vector_sink_factory<Float>::operator()(Count count) const
{
  return soa_vector_sink<Float>{hilbert::simulation::reservation_hint(count)};
}

} // namespace hilbert::simulation::suspension::sinks

#endif // HILBERT_SIMULATION_SUSPENSION_SINKS_SOA_VECTOR_HPP
