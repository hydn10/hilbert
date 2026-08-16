#ifndef HILBERT_EGEA_PROCESS_EXIT_STATUS_HPP
#define HILBERT_EGEA_PROCESS_EXIT_STATUS_HPP


#include <hilbert_egea/process/exit_domain.hpp>


namespace hilbert_egea
{

namespace failure_domain
{

struct egea
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


namespace egea
{

struct error final : process::basic_failure_outcome<failure_domain::egea, 2>
{
};

} // namespace egea


namespace critical
{

struct unhandled_exception final : process::basic_failure_outcome<failure_domain::critical, 1>
{
};

} // namespace critical

} // namespace outcome


using egea_failures = process::failure_group<failure_domain::egea, outcome::egea::error>;
using critical_failures = process::failure_group<failure_domain::critical, outcome::critical::unhandled_exception>;

struct process_exit_domain final : process::exit_domain<outcome::success, egea_failures, critical_failures>
{
};

} // namespace hilbert_egea

#endif // HILBERT_EGEA_PROCESS_EXIT_STATUS_HPP
