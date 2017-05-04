#include <iostream>
#include <cmath>
#include <datatools/units.h>
#include <genbb_help/decay0/tgold.h>

double f1(double x_, void *params_)
{
  const double * a = static_cast<double *>(params_);
  return -1 * (2.0 - (*a) * std::sin(x_));
}

int main(void)
{
  double Amplitude = 1.24;

  for (double x = 0.0; x < M_PI; x += 0.1) {
    std::cout << x << ' ' << f1(x, &Amplitude) << '\n';
  }
  std::cout << '\n' << '\n';

  double a = 0.0;
  double c = M_PI;
  double b = 0.23 * (a + c);
  double eps = 0.000001;
  int    minmax = 2; // search for maximum

  /***/
  double xextr, fextr;
  genbb::decay0::decay0_tgold(a, b, c, f1,
                              eps, minmax,
                              xextr, fextr,
                              &Amplitude);

  std::clog << "xextr=" << xextr << '\n';
  std::clog << "fextr=" << fextr
            << " (" << f1(xextr, &Amplitude) << ")\n";

  /***/
  double xextr_o, fextr_o;
  genbb::decay0::decay0_tgold_o(a, b, c, f1,
                                eps, minmax,
                                xextr_o, fextr_o,
                                &Amplitude);
  std::clog << "xextr_o=" << xextr_o << '\n';
  std::clog << "fextr_o=" << fextr_o
            << " (" << f1(xextr_o, &Amplitude) << ")\n";

  return 0;
}
