#ifndef HILBERTCLI_DERIVATIVE_HPP
#define HILBERTCLI_DERIVATIVE_HPP


#include <state.hpp>

#include <concepts>


namespace hilbertcli
{

template<std::floating_point Float>
class derivative
{
private:
  Float vphi_;
  Float vxs_;
  Float vxu_;
  Float vvs_;
  Float vvu_;

public:
  derivative(Float vphi, Float vxs, Float vxu, Float vvs, Float vvu);

  Float
  vphi() const;
  Float
  vxs() const;
  Float
  vxu() const;
  Float
  vvs() const;
  Float
  vvu() const;

  derivative
  operator+(derivative const &other) const;
  derivative
  operator*(Float scalar) const;
};


template<std::floating_point Float>
state<Float>
operator+(state<Float> const &s, derivative<Float> const &d);


template<std::floating_point Float>
derivative<Float>
operator*(Float scalar, derivative<Float> const &d);


template<std::floating_point Float>
derivative<Float>::derivative(Float vphi, Float vxs, Float vxu, Float vvs, Float vvu)
    : vphi_{vphi}
    , vxs_{vxs}
    , vxu_{vxu}
    , vvs_{vvs}
    , vvu_{vvu}
{
}


template<std::floating_point Float>
Float
derivative<Float>::vphi() const
{
  return vphi_;
}


template<std::floating_point Float>
Float
derivative<Float>::vxs() const
{
  return vxs_;
}


template<std::floating_point Float>
Float
derivative<Float>::vxu() const
{
  return vxu_;
}


template<std::floating_point Float>
Float
derivative<Float>::vvs() const
{
  return vvs_;
}


template<std::floating_point Float>
Float
derivative<Float>::vvu() const
{
  return vvu_;
}


template<std::floating_point Float>
derivative<Float>
derivative<Float>::operator+(derivative const &other) const
{
  return derivative{vphi_ + other.vphi_, vxs_ + other.vxs_, vxu_ + other.vxu_, vvs_ + other.vvs_, vvu_ + other.vvu_};
}


template<std::floating_point Float>
derivative<Float>
derivative<Float>::operator*(Float scalar) const
{
  return derivative{vphi_ * scalar, vxs_ * scalar, vxu_ * scalar, vvs_ * scalar, vvu_ * scalar};
}


template<std::floating_point Float>
state<Float>
operator+(state<Float> const &s, derivative<Float> const &d)
{
  return state{s.phi() + d.vphi(), s.xs() + d.vxs(), s.xu() + d.vxu(), s.vs() + d.vvs(), s.vu() + d.vvu()};
}


template<std::floating_point Float>
derivative<Float>
operator*(Float scalar, derivative<Float> const &d)
{
  return d * scalar;
}

} // namespace hilbertcli

#endif // HILBERTCLI_DERIVATIVE_HPP