#ifndef HILBERTCLI_STATE_HPP
#define HILBERTCLI_STATE_HPP


#include <concepts>


namespace hilbertcli
{

template<std::floating_point Float>
class state
{
private:
  Float phi_;
  Float xs_;
  Float xu_;
  Float vs_;
  Float vu_;

public:
  state(Float phi, Float xs, Float xu, Float vs, Float vu);

  Float
  phi() const;
  Float
  xs() const;
  Float
  xu() const;
  Float
  vs() const;
  Float
  vu() const;
};


template<std::floating_point Float>
state<Float>::state(Float phi, Float xs, Float xu, Float vs, Float vu)
    : phi_{phi}
    , xs_{xs}
    , xu_{xu}
    , vs_{vs}
    , vu_{vu}
{
}


template<std::floating_point Float>
Float
state<Float>::phi() const
{
  return phi_;
}


template<std::floating_point Float>
Float
state<Float>::xs() const
{
  return xs_;
}


template<std::floating_point Float>
Float
state<Float>::xu() const
{
  return xu_;
}


template<std::floating_point Float>
Float
state<Float>::vs() const
{
  return vs_;
}


template<std::floating_point Float>
Float
state<Float>::vu() const
{
  return vu_;
}

} // namespace hilbertcli

#endif // HILBERTCLI_STATE_HPP