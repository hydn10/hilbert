#ifndef HILBERT_ANALYSIS_SINUSOIDAL_BASIS_HPP
#define HILBERT_ANALYSIS_SINUSOIDAL_BASIS_HPP


#include <hilbert/analysis/least_squares/basis.hpp>
#include <hilbert/analysis/sinusoidal/frequency_hz.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/math/linear_algebra/signature.hpp>

#include <cmath>


namespace hilbert::analysis
{

struct cosine_term
{
  template<supported_float Float>
  class basis_function
  {
    Float angular_frequency_;

  public:
    using tag_type = cosine_term;

    explicit constexpr basis_function(Float angular_frequency) noexcept;

    [[nodiscard]]
    Float
    operator()(Float time) const noexcept;
  };
};


struct sine_term
{
  template<supported_float Float>
  class basis_function
  {
    Float angular_frequency_;

  public:
    using tag_type = sine_term;

    explicit constexpr basis_function(Float angular_frequency) noexcept;

    [[nodiscard]]
    Float
    operator()(Float time) const noexcept;
  };
};


struct constant_term
{
  class basis_function
  {
  public:
    using tag_type = constant_term;

    template<supported_float Float>
    [[nodiscard]]
    constexpr one_t
    operator()([[maybe_unused]] Float time) const noexcept;
  };
};


using sinusoidal_signature = math::signature<cosine_term, sine_term, constant_term>;


template<typename Signature>
concept sinusoidal_coordinate_signature =
    math::signature_for_size<Signature, 3uz> && math::contains_tag_v<Signature, cosine_term> &&
    math::contains_tag_v<Signature, sine_term> && math::contains_tag_v<Signature, constant_term>;


template<supported_float Float>
[[nodiscard]]
auto
make_sinusoidal_basis(frequency_hz<Float> const &frequency);


template<supported_float Float>
constexpr cosine_term::basis_function<Float>::basis_function(Float angular_frequency) noexcept
    : angular_frequency_{angular_frequency}
{
}


template<supported_float Float>
Float
cosine_term::basis_function<Float>::operator()(Float time) const noexcept
{
  return std::cos(angular_frequency_ * time);
}


template<supported_float Float>
constexpr sine_term::basis_function<Float>::basis_function(Float angular_frequency) noexcept
    : angular_frequency_{angular_frequency}
{
}


template<supported_float Float>
Float
sine_term::basis_function<Float>::operator()(Float time) const noexcept
{
  return std::sin(angular_frequency_ * time);
}


template<supported_float Float>
constexpr one_t
constant_term::basis_function::operator()([[maybe_unused]] Float time) const noexcept
{
  return {};
}


template<supported_float Float>
auto
make_sinusoidal_basis(frequency_hz<Float> const &frequency)
{
  auto const angular_frequency = frequency.angular_frequency();

  return basis{
      cosine_term::basis_function<Float>{angular_frequency},
      sine_term::basis_function<Float>{angular_frequency},
      constant_term::basis_function{},
  };
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SINUSOIDAL_BASIS_HPP
