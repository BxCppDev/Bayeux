// Standard library:
#include <iostream>

// Ourselves;
#include <mygsl/heaviside_function.h>

/**
  Usage:
  @code
  $ mygsl-test_heaviside_function > a.data
  $ gnuplot
  gnuplot> set grid
  gnuplot> plot 'a.data' u 1:2 with lines, '' u 1:3 w l , '' u 1:4 w l, "" u 1:5 w l
  @encode
*/
int main(void)
{
  std::clog << "test_heaviside_function: Entering...\n";

  double eps = 0.40;
  mygsl::heaviside_function H1(mygsl::heaviside_function::CONVENTION_CONTINUOUS);
  H1.set_epsilon(eps);
  mygsl::heaviside_function H2(mygsl::heaviside_function::CONVENTION_HALF_MAXIMUM);
  H2.set_epsilon(eps);
  mygsl::heaviside_function H3(mygsl::heaviside_function::CONVENTION_RIGHT_CONTINUOUS);
  H3.set_epsilon(eps);
  mygsl::heaviside_function H4(mygsl::heaviside_function::CONVENTION_LEFT_CONTINUOUS);
  H4.set_epsilon(eps);

  for (double x = -3.0; x <= +3.0; x += 0.01) {
    std::cout << x
              << ' ' << H1.eval(x)
              << ' ' << H2.eval(x)
              << ' ' << H3.eval(x)
              << ' ' << H4.eval(x)
              << std::endl;
  }

  std::clog << "test_heaviside_function: Exiting.\n";
  return 0;
}
