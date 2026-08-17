#ifndef HILBERT_APP_CLI_PARSE_HPP
#define HILBERT_APP_CLI_PARSE_HPP


#include <charconv>
#include <cmath>
#include <stdexcept>
#include <string>
#include <string_view>


namespace hilbert::app::cli
{

double
parse_positive_double(std::string_view value, std::string_view option);


inline double
parse_positive_double(std::string_view value, std::string_view option)
{
  double result{};
  auto const *const value_begin = value.data();
  auto const *const value_end = value_begin + value.size();
  auto const [parsed_end, error] = std::from_chars(value_begin, value_end, result);

  if (error != std::errc{} || parsed_end != value_end || !std::isfinite(result) || result <= 0)
  {
    throw std::invalid_argument{std::string{option} + " requires a finite, positive number"};
  }

  return result;
}

} // namespace hilbert::app::cli

#endif // HILBERT_APP_CLI_PARSE_HPP
