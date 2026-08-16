#ifndef HILBERT_EGEA_EGEA_HPP
#define HILBERT_EGEA_EGEA_HPP


#include <hilbert_egea/process/exit_status.hpp>

#include <span>


namespace hilbert_egea
{

using egea_result = process_exit_domain::result_for<failure_domain::egea>;


egea_result
run_egea(std::span<char const *const> arguments);

} // namespace hilbert_egea

#endif // HILBERT_EGEA_EGEA_HPP
