#ifndef HILBERTCLI_SIMULATION_HPP
#define HILBERTCLI_SIMULATION_HPP


#include <hilbertcli/simulation/collector.hpp>

#include <hilbert/hilbert.hpp>

#include <cstddef>


namespace hilbertcli
{

struct simulation_config
{
  double time_step = 0.0005;
  double duration = 20.0;
};


struct time_interval
{
  double start_time;
  double end_time;
};


struct simulation_result
{
  vec_collector samples;
  size_t hilbert_offset;
  size_t hilbert_size;
  time_interval measurement_interval;
  time_interval hilbert_interval;
  hilbert::signal_data<double> platform_signal;
  hilbert::signal_data<double> tire_force_signal;
};


simulation_result
run_simulation(simulation_config const &config);

} // namespace hilbertcli

#endif // HILBERTCLI_SIMULATION_HPP
