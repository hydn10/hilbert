#ifndef HILBERT_APP_CLI_PARSE_HPP
#define HILBERT_APP_CLI_PARSE_HPP


#include <expected>
#include <string_view>


namespace hilbert::app::cli
{

struct parse_error
{
};


[[nodiscard]]
std::expected<double, parse_error>
parse_positive_double(std::string_view value) noexcept;


double
require_positive_double(std::string_view value, std::string_view option);

} // namespace hilbert::app::cli

#endif // HILBERT_APP_CLI_PARSE_HPP
