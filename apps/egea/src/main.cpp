#include <hilbert/app/process/run.hpp>
#include <hilbert/egea/egea.hpp>

int
main(int argc, char const **argv)
{
  return hilbert::app::run_process(argc, argv, hilbert::egea::run_cli);
}
