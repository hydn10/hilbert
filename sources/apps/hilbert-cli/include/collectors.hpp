#ifndef HILBERTCLI_COLLECTORS_HPP
#define HILBERTCLI_COLLECTORS_HPP


#include <ranges>
#include <span>
#include <vector>


namespace hilbertcli
{

struct streamed_data_point
{
  double time;
  double xs;
  double xu;
  double ground;
  double tire_force;
};


class vec_collector
{
  std::vector<double> time_data_;
  std::vector<double> xs_data_;
  std::vector<double> xu_data_;
  std::vector<double> ground_data_;
  std::vector<double> tire_force_data_;

public:
  vec_collector(size_t size, streamed_data_point initial_data);

  vec_collector(vec_collector const &) = delete;
  vec_collector &
  operator=(vec_collector const &) = delete;

  void
  collect(streamed_data_point data_point);

  std::span<double const>
  time_span() const;
  std::span<double const>
  xs_span() const;
  std::span<double const>
  xu_span() const;
  std::span<double const>
  ground_span() const;
  std::span<double const>
  tire_force_span() const;
};

} // namespace hilbertcli

#endif
