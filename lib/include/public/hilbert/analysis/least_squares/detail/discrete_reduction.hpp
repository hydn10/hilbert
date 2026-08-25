#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_DETAIL_DISCRETE_REDUCTION_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_DETAIL_DISCRETE_REDUCTION_HPP


#include <hilbert/analysis/least_squares/basis_value.hpp>
#include <hilbert/core/supported_float.hpp>

#include <concepts>
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
  explicit constexpr known_sample_domain(std::size_t size) noexcept;

  [[nodiscard]]
  constexpr std::size_t
  size() const noexcept;
};


class counted_sample_domain
{
  std::size_t count_{};

public:
  void
  observe() noexcept;

  [[nodiscard]]
  std::size_t
  size() const noexcept;
};


template<typename Domain>
concept reduction_domain = requires(Domain const &domain) {
  { domain.size() } -> std::same_as<std::size_t>;
};


template<typename Domain>
void
observe_domain(Domain &domain) noexcept;


template<supported_float Float>
class numeric_discrete_sum
{
  Float value_{};

public:
  void
  add(Float value) noexcept;

  [[nodiscard]]
  Float
  finish() const noexcept;
};


template<supported_float Float, typename Left, typename Right>
class inner_product_term
{
  numeric_discrete_sum<Float> sum_;

public:
  void
  observe(Left const &left, Right const &right) noexcept;

  [[nodiscard]]
  Float
  finish() const noexcept;
};


template<supported_float Float>
class inner_product_term<Float, one_t, Float>
{
  numeric_discrete_sum<Float> sum_;

public:
  void
  observe(Float value) noexcept;

  [[nodiscard]]
  Float
  finish() const noexcept;
};


template<supported_float Float>
class inner_product_term<Float, Float, one_t>
{
  numeric_discrete_sum<Float> sum_;

public:
  void
  observe(Float value) noexcept;

  [[nodiscard]]
  Float
  finish() const noexcept;
};


template<supported_float Float>
class inner_product_term<Float, one_t, one_t>
{
public:
  [[nodiscard]]
  discrete_domain_measure
  finish() const noexcept;
};


template<typename Term, typename Left, typename Right>
concept product_observer = requires(Term &term, Left const &left, Right const &right) {
  { term.observe(left, right) } -> std::same_as<void>;
} || requires(Term &term, Left const &left) {
  { term.observe(left) } -> std::same_as<void>;
} || requires(Term &term, Right const &right) {
  { term.observe(right) } -> std::same_as<void>;
};


template<supported_float Float>
void
observe_product(inner_product_term<Float, one_t, one_t> &term, one_t const &left, one_t const &right) noexcept;


template<typename Term, typename Left, typename Right>
requires product_observer<Term, Left, Right>
void
observe_product(Term &term, Left const &left, Right const &right) noexcept;


template<supported_float Float, typename Domain>
[[nodiscard]]
Float
resolve_reduction(Float value, [[maybe_unused]] Domain const &domain) noexcept;


template<supported_float Float, typename Domain>
[[nodiscard]]
Float
resolve_reduction([[maybe_unused]] discrete_domain_measure measure, Domain const &domain) noexcept;


constexpr known_sample_domain::known_sample_domain(std::size_t size) noexcept
    : size_{size}
{
}


constexpr std::size_t
known_sample_domain::size() const noexcept
{
  return size_;
}


inline void
counted_sample_domain::observe() noexcept
{
  ++count_;
}


inline std::size_t
counted_sample_domain::size() const noexcept
{
  return count_;
}


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
void
numeric_discrete_sum<Float>::add(Float value) noexcept
{
  value_ += value;
}


template<supported_float Float>
Float
numeric_discrete_sum<Float>::finish() const noexcept
{
  return value_;
}


template<supported_float Float, typename Left, typename Right>
void
inner_product_term<Float, Left, Right>::observe(Left const &left, Right const &right) noexcept
{
  sum_.add(pointwise_multiply(left, right));
}


template<supported_float Float, typename Left, typename Right>
Float
inner_product_term<Float, Left, Right>::finish() const noexcept
{
  return sum_.finish();
}


template<supported_float Float>
void
inner_product_term<Float, one_t, Float>::observe(Float value) noexcept
{
  sum_.add(value);
}


template<supported_float Float>
Float
inner_product_term<Float, one_t, Float>::finish() const noexcept
{
  return sum_.finish();
}


template<supported_float Float>
void
inner_product_term<Float, Float, one_t>::observe(Float value) noexcept
{
  sum_.add(value);
}


template<supported_float Float>
Float
inner_product_term<Float, Float, one_t>::finish() const noexcept
{
  return sum_.finish();
}


template<supported_float Float>
discrete_domain_measure
inner_product_term<Float, one_t, one_t>::finish() const noexcept
{
  return {};
}


template<typename Term, typename Left, typename Right>
requires product_observer<Term, Left, Right>
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


template<supported_float Float>
void
observe_product(
    [[maybe_unused]] inner_product_term<Float, one_t, one_t> &term,
    [[maybe_unused]] one_t const &left,
    [[maybe_unused]] one_t const &right) noexcept
{
}


template<supported_float Float, typename Domain>
Float
resolve_reduction(Float value, [[maybe_unused]] Domain const &domain) noexcept
{
  return value;
}


template<supported_float Float, typename Domain>
Float
resolve_reduction([[maybe_unused]] discrete_domain_measure measure, Domain const &domain) noexcept
{
  return static_cast<Float>(domain.size());
}

} // namespace hilbert::analysis::detail

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_DETAIL_DISCRETE_REDUCTION_HPP
