#ifndef HILBERT_APP_PROCESS_RUN_HPP
#define HILBERT_APP_PROCESS_RUN_HPP


#include <hilbert/app/process/exit_status.hpp>

#include <cstddef>
#include <functional>
#include <span>
#include <utility>


namespace hilbert::app
{

template<typename RunCli>
int
run_process(int argc, char const **argv, RunCli &&run_cli) noexcept;


template<typename RunCli>
int
run_process(int argc, char const **argv, RunCli &&run_cli) noexcept
try
{
  auto const arguments = std::span<char const *const>{argv, static_cast<size_t>(argc)};
  return process_exit_domain::to_exit_code(std::invoke(std::forward<RunCli>(run_cli), arguments));
}
catch (...)
{
  return process_exit_domain::to_exit_code(outcome::critical::unhandled_exception{});
}

} // namespace hilbert::app

#endif // HILBERT_APP_PROCESS_RUN_HPP
