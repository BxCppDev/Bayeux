// test_linear_combination_function.cxx

// Ourselves:
#include <mygsl/linear_combination_function.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <mygsl/plain_function_wrapper.h>

int main(/*int argc_ , char ** argv_*/)
{
  try {

    mygsl::plain_function_wrapper f0("cos");
    mygsl::plain_function_wrapper f1("sin");
    mygsl::plain_function_wrapper f2("sqrt");

    mygsl::linear_combination_function lcf;
    int f0_index = lcf.add(+2.0, f0);
    int f1_index = lcf.add(-1.0, f1);
    int f2_index = lcf.add(+1.0, f2);
    lcf.tree_dump(std::clog, "Linear combination function: ");

    {
      double dx = 0.05;
      std::cout << "# composite function:" << std::endl;
      for (double x = 0.0; x <= 5.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << lcf(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

    lcf.change_weight(f0_index, -0.75);
    lcf.change_weight(f1_index, +1.25);
    lcf.change_weight(f2_index, 0.0);

    {
      double dx = 0.05;
      std::cout << "# composite function:" << std::endl;
      double xmin = 0.0;
      double xmax = 5.0;
      for (double x = xmin; x <= xmax + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << lcf(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
