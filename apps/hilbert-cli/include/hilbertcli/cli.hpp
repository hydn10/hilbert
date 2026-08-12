#ifndef HILBERTCLI_CLI_HPP
#define HILBERTCLI_CLI_HPP


#include <hilbertcli/process/exit_status.hpp>

#include <span>


namespace hilbertcli
{

using cli_result = process_exit_domain::result_for<failure_domain::cli>;


cli_result
run_cli(std::span<char const *const> arguments);

} // namespace hilbertcli

#endif // HILBERTCLI_CLI_HPP
