#ifndef HILBERT_APP_PROCESS_EXIT_STATUS_HPP
#define HILBERT_APP_PROCESS_EXIT_STATUS_HPP


#include <hilbert/app/process/exit_domain.hpp>


namespace hilbert::app
{

namespace failure_domain
{

struct application
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


namespace application
{

struct error final : process::basic_failure_outcome<failure_domain::application, 2>
{
};

} // namespace application


namespace critical
{

struct unhandled_exception final : process::basic_failure_outcome<failure_domain::critical, 1>
{
};

} // namespace critical

} // namespace outcome


using application_failures = process::failure_group<failure_domain::application, outcome::application::error>;
using critical_failures = process::failure_group<failure_domain::critical, outcome::critical::unhandled_exception>;

struct process_exit_domain final : process::exit_domain<outcome::success, application_failures, critical_failures>
{
};

using application_result = process_exit_domain::result_for<failure_domain::application>;
using critical_result = process_exit_domain::failures_for<failure_domain::critical>;

} // namespace hilbert::app

#endif // HILBERT_APP_PROCESS_EXIT_STATUS_HPP
