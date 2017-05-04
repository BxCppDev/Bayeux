// test_composite_function.cxx

// Ourselves:
#include <mygsl/composite_function.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <mygsl/polynomial.h>
#include <mygsl/plain_function_wrapper.h>

int main(/*int argc_ , char ** argv_*/)
{
  try {

    mygsl::polynomial p(1.0, -2.0, 0.5);
    mygsl::plain_function_wrapper pfw0("pow2");
    mygsl::plain_function_wrapper pfw1("sqrt");
    mygsl::composite_function cf(pfw0, pfw1);
    cf.tree_dump(std::clog, "Composite function: ");

    double xmin = -1.0;
    double xmax = +5.0;

    {
      double dx = 0.05;
      std::cout << "# composite function:" << std::endl;
      for (double x = xmin; x <= xmax + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << cf(x) << std::endl;
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
