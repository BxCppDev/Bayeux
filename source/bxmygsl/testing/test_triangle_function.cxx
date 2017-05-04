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

int main(/*int argc_ , char ** argv_*/)
{
  try {

    mygsl::triangle_function tf(0.2, 0.5, 0.8, 1.0);
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
