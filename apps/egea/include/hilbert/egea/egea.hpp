#ifndef HILBERT_EGEA_EGEA_HPP
#define HILBERT_EGEA_EGEA_HPP


#include <hilbert/app/process/exit_status.hpp>

#include <span>


namespace hilbert::egea
{

using egea_result = hilbert::app::application_result;


egea_result
run_cli(std::span<char const *const> arguments);

} // namespace hilbert::egea

#endif // HILBERT_EGEA_EGEA_HPP
