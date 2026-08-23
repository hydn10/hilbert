#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_BASIS3_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_BASIS3_HPP


#include <hilbert/analysis/least_squares/basis_value.hpp>
#include <hilbert/core/supported_float.hpp>
#include <hilbert/detail/attributes.hpp>

#include <concepts>
#include <type_traits>
#include <utility>


namespace hilbert::analysis
{

template<typename Value, typename Float>
concept basis_value = supported_float<Float> && (std::same_as<std::remove_cvref_t<Value>, Float> ||
                                                 std::same_as<std::remove_cvref_t<Value>, one_t>);


template<typename First, typename Second, typename Third>
class basis_row3
{
  First first_;
  Second second_;
  Third third_;

public:
  using first_type = First;
  using second_type = Second;
  using third_type = Third;

  constexpr basis_row3(First first, Second second, Third third)
      : first_{std::move(first)}
      , second_{std::move(second)}
      , third_{std::move(third)}
  {
  }

  [[nodiscard]]
  constexpr First const &
  first() const noexcept
  {
    return first_;
  }

  [[nodiscard]]
  constexpr Second const &
  second() const noexcept
  {
    return second_;
  }

  [[nodiscard]]
  constexpr Third const &
  third() const noexcept
  {
    return third_;
  }
};


template<class FirstFunction, class SecondFunction, class ThirdFunction>
class basis3
{
  HILBERT_NO_UNIQUE_ADDRESS FirstFunction first_;
  HILBERT_NO_UNIQUE_ADDRESS SecondFunction second_;
  HILBERT_NO_UNIQUE_ADDRESS ThirdFunction third_;

public:
  constexpr basis3(FirstFunction first, SecondFunction second, ThirdFunction third)
      : first_{std::move(first)}
      , second_{std::move(second)}
      , third_{std::move(third)}
  {
  }

  template<supported_float Float>
  using row_type = basis_row3<
      basis_element_value_t<Float, FirstFunction>,
      basis_element_value_t<Float, SecondFunction>,
      basis_element_value_t<Float, ThirdFunction>>;

  template<supported_float Float>
  [[nodiscard]]
  row_type<Float>
  row_at(Float argument) const
  {
    return {
        evaluate_basis_element(first_, argument),
        evaluate_basis_element(second_, argument),
        evaluate_basis_element(third_, argument),
    };
  }
};


template<class FirstFunction, class SecondFunction, class ThirdFunction>
basis3(FirstFunction, SecondFunction, ThirdFunction) -> basis3<FirstFunction, SecondFunction, ThirdFunction>;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_BASIS3_HPP
