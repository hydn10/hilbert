#ifndef HILBERTCLI_PRINTUTILS_HPP
#define HILBERTCLI_PRINTUTILS_HPP


#include <array>
#include <print>
#include <ranges>


namespace hilbertcli
{

template<size_t tokens, char delim>
consteval std::array<char, 3 * tokens - 1>
make_format_array()
{
  std::array<char, 3 * tokens - 1> arr{};
  std::array pattern = {'{', '}', delim};

  for (auto [idx, ch] : std::views::enumerate(arr))
  {
    ch = pattern[idx % 3];
  }

  return arr;
}


template<char delim, typename... Args>
void
println_join(Args... args)
{
  constexpr size_t num_tokens = sizeof...(Args);

  if constexpr (num_tokens > 0)
  {
    constexpr static auto arr = make_format_array<num_tokens, delim>();
    constexpr std::string_view fmt(arr);
    std::println(fmt, args...);
  }
  else
  {
    std::println();
  }
}

} // namespace hilbertcli

#endif
