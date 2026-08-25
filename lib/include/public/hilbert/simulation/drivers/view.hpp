#ifndef HILBERT_SIMULATION_DRIVERS_VIEW_HPP
#define HILBERT_SIMULATION_DRIVERS_VIEW_HPP


#include <hilbert/simulation/core/problem.hpp>
#include <hilbert/simulation/detail/engine.hpp>

#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <utility>


namespace hilbert::simulation
{

template<simulation_problem_for Simulation>
class simulation_view : public std::ranges::view_interface<simulation_view<Simulation>>
{
  using engine_type = detail::engine_for_t<Simulation>;

  engine_type engine_;
  std::size_t remaining_;

public:
  class sentinel;
  class iterator;

  explicit simulation_view(Simulation simulation);

  simulation_view(simulation_view const &) = delete;
  simulation_view &
  operator=(simulation_view const &) = delete;
  simulation_view(simulation_view &&) noexcept;
  simulation_view &
  operator=(simulation_view &&) noexcept;
  ~simulation_view() = default;

  iterator
  begin();

  sentinel
  end() const;

  [[nodiscard]]
  std::size_t
  size() const;
};


template<simulation_problem_for Simulation>
class simulation_view<Simulation>::sentinel
{
};


template<simulation_problem_for Simulation>
class simulation_view<Simulation>::iterator
{
  simulation_view *view_;

  [[nodiscard]]
  bool
  at_end() const;

public:
  using iterator_concept = std::input_iterator_tag;
  using iterator_category = std::input_iterator_tag;
  using value_type =
      model_sample_t<typename Simulation::model_type, typename Simulation::float_type, typename Simulation::state_type>;
  using difference_type = std::ptrdiff_t;

  value_type
  operator*() const;

  iterator &
  operator++();

  void
  operator++(int);

  friend bool
  operator==(iterator const &iterator, [[maybe_unused]] sentinel sentinel)
  {
    return iterator.at_end();
  }

  friend bool
  operator==(sentinel sentinel, iterator const &iterator)
  {
    return iterator == sentinel;
  }

private:
  explicit iterator(simulation_view *view);

  friend class simulation_view;
};


template<simulation_problem_for Simulation>
simulation_view<Simulation>
simulate(Simulation simulation);


template<simulation_problem_for Simulation>
simulation_view<Simulation>::iterator::iterator(simulation_view *view)
    : view_{view}
{
}


template<simulation_problem_for Simulation>
bool
simulation_view<Simulation>::iterator::at_end() const
{
  return view_->remaining_ == 0;
}


template<simulation_problem_for Simulation>
simulation_view<Simulation>::iterator::value_type
simulation_view<Simulation>::iterator::operator*() const
{
  return view_->engine_.current_sample();
}


template<simulation_problem_for Simulation>
simulation_view<Simulation>::iterator &
simulation_view<Simulation>::iterator::operator++()
{
  if (view_->remaining_ > 1uz)
  {
    view_->engine_.advance();
    --view_->remaining_;
  }
  else
  {
    view_->remaining_ = 0;
  }
  return *this;
}


template<simulation_problem_for Simulation>
void
simulation_view<Simulation>::iterator::operator++(int)
{
  ++*this;
}


template<simulation_problem_for Simulation>
simulation_view<Simulation>::simulation_view(Simulation simulation)
    : engine_{std::move(simulation)}
    , remaining_{engine_.sample_count()}
{
}


template<simulation_problem_for Simulation>
simulation_view<Simulation>::simulation_view(simulation_view &&) noexcept = default;


template<simulation_problem_for Simulation>
simulation_view<Simulation> &
simulation_view<Simulation>::operator=(simulation_view &&) noexcept = default;


template<simulation_problem_for Simulation>
simulation_view<Simulation>::iterator
simulation_view<Simulation>::begin()
{
  return iterator{this};
}


template<simulation_problem_for Simulation>
simulation_view<Simulation>::sentinel
simulation_view<Simulation>::end() const
{
  return {};
}


template<simulation_problem_for Simulation>
std::size_t
simulation_view<Simulation>::size() const
{
  return remaining_;
}


template<simulation_problem_for Simulation>
simulation_view<Simulation>
simulate(Simulation simulation)
{
  return simulation_view<Simulation>{std::move(simulation)};
}

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_DRIVERS_VIEW_HPP
