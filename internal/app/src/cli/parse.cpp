#include <hilbert/app/cli/parse.hpp>

#include <hilbert/app/cli/error.hpp>

#include <charconv>
#include <cmath>
#include <expected>
#include <format>
#include <memory>
#include <string_view>
#include <system_error>


namespace hilbert::app::cli
{

std::expected<double, parse_error>
parse_positive_double(std::string_view value) noexcept
{
  double result{};
  auto const *const value_begin = std::to_address(value.begin());
  auto const *const value_end = std::to_address(value.end());
  auto const [parsed_end, parse_status] = std::from_chars(value_begin, value_end, result);

  if (parse_status != std::errc{} || parsed_end != value_end || !std::isfinite(result) || result <= 0)
  {
    return std::unexpected{parse_error{}};
  }

  return result;
}


double
require_positive_double(std::string_view value, std::string_view option)
{
  auto const parsed = parse_positive_double(value);
  if (!parsed)
  {
    throw error{std::format("{} requires a finite, positive number", option)};
  }

  return *parsed;
}

} // namespace hilbert::app::cli
