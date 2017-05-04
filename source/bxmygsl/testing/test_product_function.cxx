// test_product_function.cxx

// Ourselves:
#include <mygsl/product_function.h>

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
    f0.tree_dump(std::clog, "f0 function: ");
    f1.tree_dump(std::clog, "f1 function: ");

    mygsl::product_function pf(f0, f1);
    pf.tree_dump(std::clog, "Product function: ");

    {
      double dx = 0.05;
      std::cout << "# product function:" << std::endl;
      for (double x = 0.0; x <= 5.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << pf(x) << std::endl;
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
