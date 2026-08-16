#include <hilbert_egea/egea.hpp>
#include <hilbert_egea/process/exit_status.hpp>

#include <cstddef>
#include <expected>
#include <span>


namespace
{

using critical_result = hilbert_egea::process_exit_domain::failures_for<hilbert_egea::failure_domain::critical>;
using application_result = std::expected<hilbert_egea::egea_result, critical_result>;


application_result
run_application(std::span<char const *const> arguments) noexcept
try
{
  return hilbert_egea::run_egea(arguments);
}
catch (...)
{
  return std::unexpected{critical_result{hilbert_egea::outcome::critical::unhandled_exception{}}};
}


constexpr int
application_exit_code(application_result const &result) noexcept
{
  if (result)
  {
    return hilbert_egea::process_exit_domain::to_exit_code(*result);
  }

  return hilbert_egea::process_exit_domain::to_exit_code(result.error());
}

} // namespace


int
main(int argc, char const **argv)
{
  auto const arguments = std::span<char const *const>{argv, static_cast<size_t>(argc)};
  return application_exit_code(run_application(arguments));
}
