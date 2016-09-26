// test_triangle_function.cxx

// Ourselves:
#include <mygsl/triangle_function.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/clhep_units.h>

int main(/*int argc_ , char ** argv_*/)
{
  try {

    double head_width = 0.2 * CLHEP::nanosecond;
    double tail_width = 0.5 * CLHEP::nanosecond;
    double center = 0.8 * CLHEP::nanosecond;
    double amplitude = 1.2 * CLHEP::volt;
    mygsl::triangle_function tf(head_width, tail_width, center, amplitude);
    tf.initialize_simple();
    tf.tree_dump(std::clog, "Triangle function: ");

    {
      double dx = 0.005;
      std::cout << "# triangle function:" << std::endl;
      for (double x = 0.0; x <= 5.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << tf(x) << std::endl;
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
