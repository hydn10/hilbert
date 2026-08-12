#ifndef HILBERTCLI_PROCESS_EXIT_STATUS_HPP
#define HILBERTCLI_PROCESS_EXIT_STATUS_HPP


#include <hilbertcli/process/exit_domain.hpp>


namespace hilbertcli
{

namespace failure_domain
{

struct cli
{
};


struct critical
{
};

} // namespace failure_domain


namespace outcome
{

struct success final : process::basic_success_outcome<0>
{
};


namespace cli
{

struct error final : process::basic_failure_outcome<failure_domain::cli, 2>
{
};

} // namespace cli


namespace critical
{

struct unhandled_exception final : process::basic_failure_outcome<failure_domain::critical, 1>
{
};

} // namespace critical

} // namespace outcome


using cli_failures = process::failure_group<failure_domain::cli, outcome::cli::error>;
using critical_failures = process::failure_group<failure_domain::critical, outcome::critical::unhandled_exception>;

struct process_exit_domain final : process::exit_domain<outcome::success, cli_failures, critical_failures>
{
};

} // namespace hilbertcli

#endif // HILBERTCLI_PROCESS_EXIT_STATUS_HPP
