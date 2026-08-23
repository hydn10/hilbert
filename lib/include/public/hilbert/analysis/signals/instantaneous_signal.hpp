#ifndef HILBERT_ANALYSIS_SIGNALS_INSTANTANEOUS_SIGNAL_HPP
#define HILBERT_ANALYSIS_SIGNALS_INSTANTANEOUS_SIGNAL_HPP


#include <hilbert/core/supported_float.hpp>

#include <span>
#include <stdexcept>
#include <utility>
#include <vector>


namespace hilbert
{

template<supported_float Float>
class signal_data
{
  std::vector<Float> amplitude_;
  std::vector<Float> phase_;
  std::vector<Float> frequency_;

public:
  signal_data(std::vector<Float> amplitude, std::vector<Float> phase, std::vector<Float> frequency)
      : amplitude_{std::move(amplitude)}
      , phase_{std::move(phase)}
      , frequency_{std::move(frequency)}
  {
    if (amplitude_.size() != phase_.size() || amplitude_.size() != frequency_.size())
    {
      throw std::invalid_argument{"signal data channels must have equal lengths"};
    }
  }

  [[nodiscard]]
  std::span<Float const>
  amplitude_span() const noexcept
  {
    return amplitude_;
  }

  [[nodiscard]]
  std::span<Float const>
  phase_span() const noexcept
  {
    return phase_;
  }

  [[nodiscard]]
  std::span<Float const>
  frequency_span() const noexcept
  {
    return frequency_;
  }
};


// At least two samples and a finite, positive sampling rate are required.
template<supported_float Float>
signal_data<Float>
calculate_inst_signal_data(std::span<Float const> data, Float sampling_rate);


template<supported_float Float, typename Allocator>
signal_data<Float>
calculate_inst_signal_data(std::vector<Float, Allocator> const &data, Float sampling_rate)
{
  return calculate_inst_signal_data<Float>(std::span<Float const>{data}, sampling_rate);
}


extern template signal_data<double>
calculate_inst_signal_data<double>(std::span<double const> data, double sampling_rate);

} // namespace hilbert

#endif // HILBERT_ANALYSIS_SIGNALS_INSTANTANEOUS_SIGNAL_HPP
