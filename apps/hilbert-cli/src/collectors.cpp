#include <collectors.hpp>

#include <print>
#include <span>


namespace hilbertcli
{

vec_collector::vec_collector(size_t size, streamed_data_point initial_data)
{
  time_data_.reserve(size);
  xs_data_.reserve(size);
  xu_data_.reserve(size);
  ground_data_.reserve(size);
  tire_force_data_.reserve(size);

  collect(initial_data);
}


void
vec_collector::collect(streamed_data_point data_point)
{
  time_data_.emplace_back(std::move(data_point.time));
  xs_data_.emplace_back(std::move(data_point.xs));
  xu_data_.emplace_back(std::move(data_point.xu));
  ground_data_.emplace_back(std::move(data_point.ground));
  tire_force_data_.emplace_back(std::move(data_point.tire_force));
}


std::span<double const>
vec_collector::time_span() const
{
  return time_data_;
}


std::span<double const>
vec_collector::xs_span() const
{
  return xs_data_;
}


std::span<double const>
vec_collector::xu_span() const
{
  return xu_data_;
}


std::span<double const>
vec_collector::ground_span() const
{
  return ground_data_;
}


std::span<double const>
vec_collector::tire_force_span() const
{
  return tire_force_data_;
}

} // namespace hilbertcli
