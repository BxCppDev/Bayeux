#include <iostream>
#include <cmath>
#include <genbb_help/decay0/fermi.h>

int main(void)
{
  double Z = 44.0;
  for (double e = 0.005; e <= 4.0; e += 0.025) {
    std::cout << e
              << ' ' << genbb::decay0::decay0_fermi(Z,e)
              << ' ' << genbb::decay0::decay0_fermi_func_nr_approx(Z,e)
              << ' ' << genbb::decay0::decay0_fermi_func(Z,e)
              << '\n';
  }

  return 0;
}
