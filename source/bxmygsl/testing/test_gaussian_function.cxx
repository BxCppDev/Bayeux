// test_gaussian_function.cxx

// Ourselves:
#include <mygsl/gaussian_function.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

int main(/*int argc_ , char ** argv_*/)
{
  try {

    mygsl::gaussian_function gf(0.6, 2.3);
    gf.initialize_simple();
    gf.tree_dump(std::clog, "Gaussian function: ");

    {
      double dx = 0.005;
      std::cout << "# gaussian function:" << std::endl;
      for (double x = 0.0; x <= 5.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << gf(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
