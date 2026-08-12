#include <hilbertcli/cli.hpp>
#include <hilbertcli/process/exit_status.hpp>

#include <cstddef>
#include <expected>
#include <span>


namespace
{

using critical_result = hilbertcli::process_exit_domain::failures_for<hilbertcli::failure_domain::critical>;
using application_result = std::expected<hilbertcli::cli_result, critical_result>;


application_result
run_application(std::span<char const *const> arguments) noexcept
try
{
  return hilbertcli::run_cli(arguments);
}
catch (...)
{
  return std::unexpected{critical_result{hilbertcli::outcome::critical::unhandled_exception{}}};
}


constexpr int
application_exit_code(application_result const &result) noexcept
{
  if (result)
  {
    return hilbertcli::process_exit_domain::to_exit_code(*result);
  }

  return hilbertcli::process_exit_domain::to_exit_code(result.error());
}

} // namespace


int
main(int argc, char const **argv)
{
  auto const arguments = std::span<char const *const>{argv, static_cast<size_t>(argc)};
  return application_exit_code(run_application(arguments));
}
