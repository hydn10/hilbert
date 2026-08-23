#ifndef HILBERT_ANALYSIS_SAMPLING_SAMPLE_WINDOW_HPP
#define HILBERT_ANALYSIS_SAMPLING_SAMPLE_WINDOW_HPP


#include <hilbert/core/supported_float.hpp>

#include <cmath>
#include <cstddef>
#include <iterator>
#include <span>
#include <stdexcept>


namespace hilbert::analysis
{

template<supported_float Float>
class time_window
{
  Float begin_;
  Float end_;

public:
  time_window(Float begin, Float end)
      : begin_{begin}
      , end_{end}
  {
    if (!std::isfinite(begin_) || !std::isfinite(end_) || begin_ >= end_)
    {
      throw std::invalid_argument{"time window must have finite, increasing bounds"};
    }
  }

  [[nodiscard]]
  Float
  begin() const noexcept
  {
    return begin_;
  }

  [[nodiscard]]
  Float
  end() const noexcept
  {
    return end_;
  }
};


class sample_range
{
  std::size_t begin_;
  std::size_t end_;
  std::size_t record_size_;

  sample_range(std::size_t begin, std::size_t end, std::size_t record_size) noexcept
      : begin_{begin}
      , end_{end}
      , record_size_{record_size}
  {
  }

  template<supported_float Float>
  friend sample_range
  select_sample_range(std::span<Float const> time, time_window<Float> window);

public:
  [[nodiscard]]
  std::size_t
  begin() const noexcept
  {
    return begin_;
  }

  [[nodiscard]]
  std::size_t
  end() const noexcept
  {
    return end_;
  }

  [[nodiscard]]
  std::size_t
  size() const noexcept
  {
    return end_ - begin_;
  }

  template<typename T>
  [[nodiscard]]
  std::span<T>
  slice(std::span<T> values) const
  {
    if (values.size() != record_size_)
    {
      throw std::invalid_argument{"sample range does not match record length"};
    }
    return values.subspan(begin_, size());
  }
};


template<supported_float Float>
sample_range
select_sample_range(std::span<Float const> time, time_window<Float> window)
{
  if (time.size() < 2uz)
  {
    throw std::invalid_argument{"phase analysis requires at least two time samples"};
  }

  auto previous = time.begin();
  for (auto current = time.begin(); current != time.end(); ++current)
  {
    if (!std::isfinite(*current) || (current != time.begin() && *current <= *previous))
    {
      throw std::invalid_argument{"time samples must be finite and strictly increasing"};
    }
    previous = current;
  }

  auto begin_iterator = time.begin();
  while (begin_iterator != time.end() && *begin_iterator < window.begin())
  {
    ++begin_iterator;
  }

  auto end_iterator = begin_iterator;
  while (end_iterator != time.end() && *end_iterator < window.end())
  {
    ++end_iterator;
  }

  auto const begin = static_cast<std::size_t>(std::distance(time.begin(), begin_iterator));
  auto const end = static_cast<std::size_t>(std::distance(time.begin(), end_iterator));
  if (end - begin < 2uz)
  {
    throw std::invalid_argument{"time window contains too few samples"};
  }
  return sample_range{begin, end, time.size()};
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SAMPLING_SAMPLE_WINDOW_HPP
