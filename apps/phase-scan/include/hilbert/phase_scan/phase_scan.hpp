#ifndef HILBERT_PHASE_SCAN_PHASE_SCAN_HPP
#define HILBERT_PHASE_SCAN_PHASE_SCAN_HPP


#include <hilbert/app/process/exit_status.hpp>

#include <span>


namespace hilbert::phase_scan
{

hilbert::app::application_result
run_cli(std::span<char const *const> arguments);

} // namespace hilbert::phase_scan

#endif // HILBERT_PHASE_SCAN_PHASE_SCAN_HPP
