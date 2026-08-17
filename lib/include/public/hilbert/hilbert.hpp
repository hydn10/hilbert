#ifndef HILBERT_HILBERT_HPP
#define HILBERT_HILBERT_HPP


#include <complex>
#include <concepts>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>


namespace hilbert
{

template<typename Float>
concept supported_float = std::same_as<Float, double>;


template<std::floating_point Float>
class signal_data
{
  std::vector<Float> amplitude_;
  std::vector<Float> phase_;
  std::vector<Float> frequency_;

public:
  signal_data(std::vector<Float> amplitude, std::vector<Float> phase, std::vector<Float> frequency);

  [[nodiscard]]
  std::span<Float const>
  amplitude_span() const noexcept;

  [[nodiscard]]
  std::span<Float const>
  phase_span() const noexcept;

  [[nodiscard]]
  std::span<Float const>
  frequency_span() const noexcept;
};


// At least two samples are required. Throws std::invalid_argument otherwise. Float must be a supported floating type.
template<supported_float Float>
std::vector<std::complex<Float>>
hilbert_transform(std::span<Float const> input);


// At least two samples and a finite, positive sampling rate are required. Float must be a supported floating type.
// Throws std::invalid_argument when either precondition is not satisfied.
template<supported_float Float>
signal_data<Float>
calculate_inst_signal_data(std::span<Float const> data, Float sampling_rate);


template<supported_float Float, typename Allocator>
std::vector<std::complex<Float>>
hilbert_transform(std::vector<Float, Allocator> const &input);


template<supported_float Float, typename Allocator>
signal_data<Float>
calculate_inst_signal_data(std::vector<Float, Allocator> const &data, Float sampling_rate);


extern template std::vector<std::complex<double>>
hilbert_transform<double>(std::span<double const> input);

extern template signal_data<double>
calculate_inst_signal_data<double>(std::span<double const> data, double sampling_rate);


template<supported_float Float, typename Allocator>
std::vector<std::complex<Float>>
hilbert_transform(std::vector<Float, Allocator> const &input)
{
  return hilbert_transform<Float>(std::span<Float const>{input});
}


template<supported_float Float, typename Allocator>
signal_data<Float>
calculate_inst_signal_data(std::vector<Float, Allocator> const &data, Float sampling_rate)
{
  return calculate_inst_signal_data<Float>(std::span<Float const>{data}, sampling_rate);
}


template<std::floating_point Float>
signal_data<Float>::signal_data(std::vector<Float> amplitude, std::vector<Float> phase, std::vector<Float> frequency)
    : amplitude_{std::move(amplitude)}
    , phase_{std::move(phase)}
    , frequency_{std::move(frequency)}
{
  if (amplitude_.size() != phase_.size() || amplitude_.size() != frequency_.size())
  {
    throw std::invalid_argument{"signal data channels must have equal lengths"};
  }
}


template<std::floating_point Float>
std::span<Float const>
signal_data<Float>::amplitude_span() const noexcept
{
  return amplitude_;
}


template<std::floating_point Float>
std::span<Float const>
signal_data<Float>::phase_span() const noexcept
{
  return phase_;
}


template<std::floating_point Float>
std::span<Float const>
signal_data<Float>::frequency_span() const noexcept
{
  return frequency_;
}

} // namespace hilbert

#endif
