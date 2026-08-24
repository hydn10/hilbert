#ifndef HILBERT_ANALYSIS_SAMPLING_SAMPLE_WINDOW_HPP
#define HILBERT_ANALYSIS_SAMPLING_SAMPLE_WINDOW_HPP


#include <hilbert/core/supported_float.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
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


// Precondition: time contains finite values sorted in nondecreasing order.
// TODO: Express this precondition as a contract when supported.
template<supported_float Float>
sample_range
select_sample_range(std::span<Float const> time, time_window<Float> window)
{
  if (time.size() < 2uz)
  {
    throw std::invalid_argument{"phase analysis requires at least two time samples"};
  }

  auto const begin_iterator = std::ranges::lower_bound(time, window.begin());
  auto const end_iterator = std::ranges::lower_bound(begin_iterator, time.end(), window.end());

  if (end_iterator - begin_iterator < 2)
  {
    throw std::invalid_argument{"time window contains too few samples"};
  }

  auto const begin = static_cast<std::size_t>(begin_iterator - time.begin());
  auto const end = static_cast<std::size_t>(end_iterator - time.begin());

  return sample_range{begin, end, time.size()};
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SAMPLING_SAMPLE_WINDOW_HPP
