#ifndef HILBERT_APP_CLI_ARGUMENT_CURSOR_HPP
#define HILBERT_APP_CLI_ARGUMENT_CURSOR_HPP


#include <span>
#include <string_view>


namespace hilbert::app::cli
{

class argument_cursor
{
  std::span<char const *const> arguments_;

public:
  explicit argument_cursor(std::span<char const *const> arguments) noexcept;

  explicit
  operator bool() const noexcept;

  std::string_view
  next() noexcept;

  std::string_view
  require_value(std::string_view option);
};

} // namespace hilbert::app::cli

#endif // HILBERT_APP_CLI_ARGUMENT_CURSOR_HPP
