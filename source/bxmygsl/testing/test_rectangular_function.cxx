// Standard library:
#include <iostream>

// Ourselves;
#include <mygsl/rectangular_function.h>


/**
  Usage:
  @code
  $ mygsl-test_rectangular_function > a.data
  $ gnuplot
  gnuplot> set grid
  gnuplot> plot 'a.data' u 1:2 with lines, '' u 1:3 w l , '' u 1:4 w l, "" u 1:5 w l
  @encode
*/
int main(void)
{
  std::clog << "test_rectangular_function: Entering...\n";

  double eps = 0.10;
  mygsl::rectangular_function Pi1(mygsl::rectangular_function::CONVENTION_CONTINUOUS);
  Pi1.set_epsilon(eps);
  Pi1.tree_dump(std::clog, "Pi(x):");

  mygsl::rectangular_function Pi2(mygsl::rectangular_function::CONVENTION_HALF_MAXIMUM);
  Pi2.set_epsilon(eps);

  mygsl::rectangular_function Pi3(mygsl::rectangular_function::CONVENTION_INNER_CONTINUOUS);
  Pi3.set_epsilon(eps);

  mygsl::rectangular_function Pi4(mygsl::rectangular_function::CONVENTION_OUTER_CONTINUOUS);
  Pi4.set_epsilon(eps);

  for (double x = -3.0; x <= +3.0; x += 0.01) {
    std::cout << x
              << ' ' << Pi1(x)
              << ' ' << Pi2(x)
              << ' ' << Pi3(x)
              << ' ' << Pi4(x)
              << std::endl;
  }

  std::clog << "test_rectangular_function: Exiting.\n";
  return 0;
}
