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

template<typename Simulation>
requires simulation_problem_for<Simulation>
class simulation_view : public std::ranges::view_interface<simulation_view<Simulation>>
{
  using engine_type = detail::engine_for_t<Simulation>;

  engine_type engine_;
  std::size_t remaining_;

public:
  class sentinel
  {
  };

  class iterator
  {
    simulation_view *view_;

    bool
    at_end() const
    {
      return view_->remaining_ == 0;
    }

  public:
    using iterator_concept = std::input_iterator_tag;
    using iterator_category = std::input_iterator_tag;
    using value_type = model_sample_t<
        typename Simulation::model_type,
        typename Simulation::float_type,
        typename Simulation::state_type>;
    using difference_type = std::ptrdiff_t;

    value_type
    operator*() const
    {
      return view_->engine_.current_sample();
    }

    iterator &
    operator++()
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

    void
    operator++(int)
    {
      ++*this;
    }

    friend bool
    operator==(iterator const &iterator, sentinel)
    {
      return iterator.at_end();
    }

    friend bool
    operator==(sentinel sentinel, iterator const &iterator)
    {
      return iterator == sentinel;
    }

  private:
    explicit iterator(simulation_view *view)
        : view_{view}
    {
    }

    friend class simulation_view;
  };

  explicit simulation_view(Simulation simulation)
      : engine_{detail::make_engine(std::move(simulation))}
      , remaining_{engine_.sample_count()}
  {
  }

  simulation_view(simulation_view const &) = delete;
  simulation_view &
  operator=(simulation_view const &) = delete;
  simulation_view(simulation_view &&) = default;
  simulation_view &
  operator=(simulation_view &&) = default;

  iterator
  begin()
  {
    return iterator{this};
  }

  sentinel
  end() const
  {
    return {};
  }

  std::size_t
  size() const
  {
    return remaining_;
  }
};


template<typename Simulation>
requires simulation_problem_for<Simulation>
simulation_view<Simulation>
simulate(Simulation simulation)
{
  return simulation_view<Simulation>{std::move(simulation)};
}

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_DRIVERS_VIEW_HPP
