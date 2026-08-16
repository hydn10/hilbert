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
  using state_type = detail::suspension_state<Float>;
  using model_type = detail::suspension_model<Float, FrequencyProfile>;
  using engine_type = detail::simulation_engine<Float, state_type, model_type, detail::rk4>;

  engine_type engine_;
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

  explicit simulation_view(engine_type engine)
      : engine_{std::move(engine)}
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
  return simulation_view<Float, FrequencyProfile>{
      detail::make_suspension_engine(simulation_config, std::move(frequency_profile))};
}


using simulation_view_concept_check_profile = decltype([](double) { return 0.0; });

static_assert(std::ranges::input_range<simulation_view<double, simulation_view_concept_check_profile>>);
static_assert(std::ranges::sized_range<simulation_view<double, simulation_view_concept_check_profile>>);
static_assert(std::ranges::view<simulation_view<double, simulation_view_concept_check_profile>>);
static_assert(!std::ranges::forward_range<simulation_view<double, simulation_view_concept_check_profile>>);

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_VIEW_HPP
