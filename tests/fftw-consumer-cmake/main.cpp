#include <fftw3.h>

int
main()
{
  fftw_complex input[1]{};
  fftw_complex output[1]{};
  fftw_plan plan = fftw_plan_dft_1d(1, input, output, FFTW_FORWARD, FFTW_ESTIMATE);
  if (plan == nullptr)
  {
    return 1;
  }

  fftw_execute(plan);
  fftw_destroy_plan(plan);
  return 0;
}
