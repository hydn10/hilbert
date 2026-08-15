#ifndef HILBERT_SIMULATION_VIEW_HPP
#define HILBERT_SIMULATION_VIEW_HPP


#include <hilbert/simulation/detail/evolution.hpp>

#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <utility>


namespace hilbert::simulation
{

template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
class simulation_view : public std::ranges::view_interface<simulation_view<Float, FrequencyProfile>>
{
  using state_type = detail::simulation_state<Float, FrequencyProfile>;

  state_type state_;
  size_t remaining_;

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
    using value_type = sample<Float>;
    using difference_type = std::ptrdiff_t;

    value_type
    operator*() const
    {
      return detail::observe_simulation(view_->state_);
    }

    iterator &
    operator++()
    {
      if (view_->remaining_ > 1uz)
      {
        detail::advance_simulation(view_->state_);
        --view_->remaining_;
      }
      else
      {
        view_->remaining_ = 0;
      }
      return *this;
    }

    iterator
    operator++(int)
    {
      auto copy = *this;
      ++*this;
      return copy;
    }

    friend bool
    operator==(iterator const &left, iterator const &right)
    {
      return left.view_ == right.view_ && left.view_->remaining_ == right.view_->remaining_;
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

  explicit simulation_view(detail::initialized_simulation<state_type> initialized)
      : state_{std::move(initialized.state)}
      , remaining_{initialized.sample_count}
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

  size_t
  size() const
  {
    return remaining_;
  }
};


template<std::floating_point Float, frequency::profile<Float> FrequencyProfile>
simulation_view<Float, FrequencyProfile>
simulate(config<Float> const &simulation_config, FrequencyProfile frequency_profile)
{
  auto initialized = detail::initialize_simulation(
      simulation_config, std::move(frequency_profile), detail::suspension_state<Float>{0, 0, 0, 0, 0});
  return simulation_view<Float, FrequencyProfile>{std::move(initialized)};
}


using simulation_view_concept_check_profile = decltype([](double) { return 0.0; });

static_assert(std::ranges::input_range<simulation_view<double, simulation_view_concept_check_profile>>);
static_assert(std::ranges::sized_range<simulation_view<double, simulation_view_concept_check_profile>>);
static_assert(std::ranges::view<simulation_view<double, simulation_view_concept_check_profile>>);
static_assert(!std::ranges::forward_range<simulation_view<double, simulation_view_concept_check_profile>>);

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_VIEW_HPP
