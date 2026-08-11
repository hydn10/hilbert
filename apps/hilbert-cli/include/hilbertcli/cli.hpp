#ifndef HILBERTCLI_CLI_HPP
#define HILBERTCLI_CLI_HPP


#include <hilbertcli/exit_status.hpp>


namespace hilbertcli
{

exit_status
run_cli(int argc, char const **argv);

} // namespace hilbertcli

#endif // HILBERTCLI_CLI_HPP
