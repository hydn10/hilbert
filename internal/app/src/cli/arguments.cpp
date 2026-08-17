#include <hilbert/app/cli/arguments.hpp>
#include <hilbert/app/cli/error.hpp>

#include <format>
#include <span>
#include <string_view>


namespace hilbert::app::cli
{

argument_cursor::argument_cursor(std::span<char const *const> arguments) noexcept
    : arguments_{arguments}
{
}


argument_cursor::
operator bool() const noexcept
{
  return !arguments_.empty();
}


std::string_view
argument_cursor::next() noexcept
{
  auto const argument = std::string_view{arguments_.front()};
  arguments_ = arguments_.subspan(1);
  return argument;
}


std::string_view
argument_cursor::require_value(std::string_view option)
{
  if (arguments_.empty())
  {
    throw error{std::format("missing value for {}", option)};
  }

  return next();
}

} // namespace hilbert::app::cli
