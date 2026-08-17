#ifndef HILBERT_APP_CLI_RUN_HPP
#define HILBERT_APP_CLI_RUN_HPP


#include <hilbert/app/process/exit_status.hpp>

#include <exception>
#include <functional>
#include <iostream>
#include <print>
#include <string_view>
#include <utility>


namespace hilbert::app::cli
{

template<typename RunCommand, typename PrintUsageSynopsis>
application_result
run(std::string_view application_name,
    std::ostream &error_output,
    RunCommand &&run_command,
    PrintUsageSynopsis &&print_usage_synopsis);


template<typename RunCommand, typename PrintUsageSynopsis>
application_result
run(std::string_view application_name,
    std::ostream &error_output,
    RunCommand &&run_command,
    PrintUsageSynopsis &&print_usage_synopsis)
try
{
  std::invoke(std::forward<RunCommand>(run_command));
  return outcome::success{};
}
catch (std::exception const &error)
{
  std::println(error_output, "{}: {}", application_name, error.what());
  std::invoke(std::forward<PrintUsageSynopsis>(print_usage_synopsis), error_output);
  return outcome::application::error{};
}

} // namespace hilbert::app::cli

#endif // HILBERT_APP_CLI_RUN_HPP
