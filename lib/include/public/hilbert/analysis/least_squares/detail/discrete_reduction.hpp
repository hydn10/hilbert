#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_DETAIL_DISCRETE_REDUCTION_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_DETAIL_DISCRETE_REDUCTION_HPP


#include <hilbert/analysis/least_squares/basis_value.hpp>
#include <hilbert/core/supported_float.hpp>

#include <cstddef>


namespace hilbert::analysis::detail
{

class discrete_domain_measure
{
};


class known_sample_domain
{
  std::size_t size_;

public:
  explicit constexpr known_sample_domain(std::size_t size) noexcept
      : size_{size}
  {
  }

  [[nodiscard]]
  constexpr std::size_t
  size() const noexcept
  {
    return size_;
  }
};


class counted_sample_domain
{
  std::size_t count_{};

public:
  void
  observe() noexcept
  {
    ++count_;
  }

  [[nodiscard]]
  std::size_t
  size() const noexcept
  {
    return count_;
  }
};


template<typename Domain>
void
observe_domain(Domain &domain) noexcept
{
  if constexpr (requires { domain.observe(); })
  {
    domain.observe();
  }
}


template<supported_float Float>
class numeric_discrete_sum
{
  Float value_{};

public:
  void
  add(Float value) noexcept
  {
    value_ += value;
  }

  [[nodiscard]]
  Float
  finish() const noexcept
  {
    return value_;
  }
};


template<supported_float Float, typename Left, typename Right>
class inner_product_term
{
  numeric_discrete_sum<Float> sum_;

public:
  void
  observe(Left const &left, Right const &right) noexcept
  {
    sum_.add(pointwise_multiply(left, right));
  }

  [[nodiscard]]
  Float
  finish() const noexcept
  {
    return sum_.finish();
  }
};


template<supported_float Float>
class inner_product_term<Float, one_t, Float>
{
  numeric_discrete_sum<Float> sum_;

public:
  void
  observe(Float value) noexcept
  {
    sum_.add(value);
  }

  [[nodiscard]]
  Float
  finish() const noexcept
  {
    return sum_.finish();
  }
};


template<supported_float Float>
class inner_product_term<Float, Float, one_t>
{
  numeric_discrete_sum<Float> sum_;

public:
  void
  observe(Float value) noexcept
  {
    sum_.add(value);
  }

  [[nodiscard]]
  Float
  finish() const noexcept
  {
    return sum_.finish();
  }
};


template<supported_float Float>
class inner_product_term<Float, one_t, one_t>
{
public:
  [[nodiscard]]
  discrete_domain_measure
  finish() const noexcept
  {
    return {};
  }
};


template<typename Term, typename Left, typename Right>
void
observe_product(Term &term, Left const &left, Right const &right) noexcept
{
  if constexpr (requires { term.observe(left, right); })
  {
    term.observe(left, right);
  }
  else if constexpr (requires { term.observe(left); })
  {
    term.observe(left);
  }
  else if constexpr (requires { term.observe(right); })
  {
    term.observe(right);
  }
}


template<supported_float Float, typename Domain>
[[nodiscard]]
Float
resolve_reduction(Float value, [[maybe_unused]] Domain const &domain) noexcept
{
  return value;
}


template<supported_float Float, typename Domain>
[[nodiscard]]
Float
resolve_reduction([[maybe_unused]] discrete_domain_measure measure, Domain const &domain) noexcept
{
  return static_cast<Float>(domain.size());
}

} // namespace hilbert::analysis::detail

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_DETAIL_DISCRETE_REDUCTION_HPP
