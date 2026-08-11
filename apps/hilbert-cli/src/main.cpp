#include <hilbertcli/cli.hpp>
#include <hilbertcli/exit_status.hpp>


int
main(int argc, char const **argv)
{
  try
  {
    return hilbertcli::to_exit_code(hilbertcli::run_cli(argc, argv));
  }
  catch (...)
  {
    return hilbertcli::to_exit_code(hilbertcli::exit_status::critical_error);
  }
}
