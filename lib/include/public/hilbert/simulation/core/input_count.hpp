#ifndef HILBERT_SIMULATION_CORE_INPUT_COUNT_HPP
#define HILBERT_SIMULATION_CORE_INPUT_COUNT_HPP


#include <concepts>
#include <cstddef>
#include <type_traits>


namespace hilbert::simulation
{

// The factory promises that, if the resulting sink reaches finish(), exactly
// value() input samples have been accepted successfully before finish() is called.
class exact_input_count
{
  std::size_t value_;

public:
  explicit constexpr exact_input_count(std::size_t value) noexcept;

  [[nodiscard]]
  constexpr std::size_t
  value() const noexcept;
};


class input_count_upper_bound
{
  std::size_t value_;

public:
  explicit constexpr input_count_upper_bound(std::size_t value) noexcept;

  [[nodiscard]]
  constexpr std::size_t
  value() const noexcept;
};


struct unknown_input_count
{
};


template<typename Count>
concept input_count_descriptor = std::same_as<std::remove_cvref_t<Count>, exact_input_count> ||
                                 std::same_as<std::remove_cvref_t<Count>, input_count_upper_bound> ||
                                 std::same_as<std::remove_cvref_t<Count>, unknown_input_count>;


[[nodiscard]]
constexpr input_count_upper_bound
as_upper_bound(exact_input_count count) noexcept;


[[nodiscard]]
constexpr input_count_upper_bound
as_upper_bound(input_count_upper_bound count) noexcept;


[[nodiscard]]
constexpr unknown_input_count
as_upper_bound([[maybe_unused]] unknown_input_count count) noexcept;


[[nodiscard]]
constexpr std::size_t
reservation_hint(exact_input_count count) noexcept;


[[nodiscard]]
constexpr std::size_t
reservation_hint(input_count_upper_bound count) noexcept;


[[nodiscard]]
constexpr std::size_t
reservation_hint([[maybe_unused]] unknown_input_count count) noexcept;


constexpr exact_input_count::exact_input_count(std::size_t value) noexcept
    : value_{value}
{
}


constexpr std::size_t
exact_input_count::value() const noexcept
{
  return value_;
}


constexpr input_count_upper_bound::input_count_upper_bound(std::size_t value) noexcept
    : value_{value}
{
}


constexpr std::size_t
input_count_upper_bound::value() const noexcept
{
  return value_;
}


constexpr input_count_upper_bound
as_upper_bound(exact_input_count count) noexcept
{
  return input_count_upper_bound{count.value()};
}


constexpr input_count_upper_bound
as_upper_bound(input_count_upper_bound count) noexcept
{
  return count;
}


constexpr unknown_input_count
as_upper_bound([[maybe_unused]] unknown_input_count count) noexcept
{
  return {};
}


constexpr std::size_t
reservation_hint(exact_input_count count) noexcept
{
  return count.value();
}


constexpr std::size_t
reservation_hint(input_count_upper_bound count) noexcept
{
  return count.value();
}


constexpr std::size_t
reservation_hint([[maybe_unused]] unknown_input_count count) noexcept
{
  return 0uz;
}

} // namespace hilbert::simulation

#endif // HILBERT_SIMULATION_CORE_INPUT_COUNT_HPP
