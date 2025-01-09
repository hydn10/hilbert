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
  state(Float phi, Float xs, Float xu, Float vs, Float vu)
      : phi_{phi}
      , xs_{xs}
      , xu_{xu}
      , vs_{vs}
      , vu_{vu}
  {
  }

  Float
  phi() const
  {
    return phi_;
  }

  Float
  xs() const
  {
    return xs_;
  }
  Float
  xu() const
  {
    return xu_;
  }
  Float
  vs() const
  {
    return vs_;
  }
  Float
  vu() const
  {
    return vu_;
  }
};

} // namespace hilbertcli

#endif // HILBERTCLI_STATE_HPP