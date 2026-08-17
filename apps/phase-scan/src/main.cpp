#include <hilbert/app/process/run.hpp>
#include <hilbert/phase_scan/phase_scan.hpp>

int
main(int argc, char const **argv)
{
  return hilbert::app::run_process(argc, argv, hilbert::phase_scan::run_cli);
}
