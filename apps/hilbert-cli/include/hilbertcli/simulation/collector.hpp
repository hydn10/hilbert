#ifndef HILBERTCLI_SIMULATION_COLLECTOR_HPP
#define HILBERTCLI_SIMULATION_COLLECTOR_HPP


#include <concepts>
#include <cstddef>
#include <span>
#include <vector>


namespace hilbertcli
{

template<std::floating_point Float>
struct streamed_data_point
{
  Float time;
  Float xs;
  Float xu;
  Float ground;
  Float tire_force;
};


template<std::floating_point Float>
class vec_collector
{
  std::vector<Float> time_data_;
  std::vector<Float> xs_data_;
  std::vector<Float> xu_data_;
  std::vector<Float> ground_data_;
  std::vector<Float> tire_force_data_;

public:
  vec_collector(size_t size, streamed_data_point<Float> initial_data);

  vec_collector(vec_collector const &) = delete;
  vec_collector &
  operator=(vec_collector const &) = delete;
  vec_collector(vec_collector &&) noexcept = default;
  vec_collector &
  operator=(vec_collector &&) noexcept = default;

  void
  collect(streamed_data_point<Float> data_point);

  std::span<Float const>
  time_span() const;
  std::span<Float const>
  xs_span() const;
  std::span<Float const>
  xu_span() const;
  std::span<Float const>
  ground_span() const;
  std::span<Float const>
  tire_force_span() const;
};


template<std::floating_point Float>
vec_collector<Float>::vec_collector(size_t size, streamed_data_point<Float> initial_data)
{
  time_data_.reserve(size);
  xs_data_.reserve(size);
  xu_data_.reserve(size);
  ground_data_.reserve(size);
  tire_force_data_.reserve(size);

  collect(initial_data);
}


template<std::floating_point Float>
void
vec_collector<Float>::collect(streamed_data_point<Float> data_point)
{
  time_data_.emplace_back(data_point.time);
  xs_data_.emplace_back(data_point.xs);
  xu_data_.emplace_back(data_point.xu);
  ground_data_.emplace_back(data_point.ground);
  tire_force_data_.emplace_back(data_point.tire_force);
}


template<std::floating_point Float>
std::span<Float const>
vec_collector<Float>::time_span() const
{
  return time_data_;
}


template<std::floating_point Float>
std::span<Float const>
vec_collector<Float>::xs_span() const
{
  return xs_data_;
}


template<std::floating_point Float>
std::span<Float const>
vec_collector<Float>::xu_span() const
{
  return xu_data_;
}


template<std::floating_point Float>
std::span<Float const>
vec_collector<Float>::ground_span() const
{
  return ground_data_;
}


template<std::floating_point Float>
std::span<Float const>
vec_collector<Float>::tire_force_span() const
{
  return tire_force_data_;
}

} // namespace hilbertcli

#endif // HILBERTCLI_SIMULATION_COLLECTOR_HPP
