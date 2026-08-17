#include <hilbert/app/process/exit_status.hpp>
#include <hilbert/phase_scan/phase_scan.hpp>

#include <cstddef>
#include <expected>
#include <span>


namespace
{

using critical_result = hilbert::app::critical_result;
using application_result = std::expected<hilbert::app::application_result, critical_result>;


application_result
run_application(std::span<char const *const> arguments) noexcept
try
{
  return hilbert::phase_scan::run_cli(arguments);
}
catch (...)
{
  return std::unexpected{critical_result{hilbert::app::outcome::critical::unhandled_exception{}}};
}


constexpr int
application_exit_code(application_result const &result) noexcept
{
  if (result)
  {
    return hilbert::app::process_exit_domain::to_exit_code(*result);
  }

  return hilbert::app::process_exit_domain::to_exit_code(result.error());
}

} // namespace


int
main(int argc, char const **argv)
{
  auto const arguments = std::span<char const *const>{argv, static_cast<size_t>(argc)};
  return application_exit_code(run_application(arguments));
}
