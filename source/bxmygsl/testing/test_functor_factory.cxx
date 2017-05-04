// test_functor_factory.cxx

// Ourselves:
#include <mygsl/functor_factory.h>

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

    mygsl::functor_factory ff;
    ff.tree_dump(std::clog);
    ff.lock();

    std::string functor_id;
    datatools::properties functor_config;

    {
      functor_id = "mygsl::polynomial";
      std::vector<double> coefs;
      coefs.push_back(+1.0);
      coefs.push_back(-2.0);
      coefs.push_back(+0.5);
      functor_config.store("coefficients", coefs);

      mygsl::unary_function_handle_type functor_handle;
      ff.create(functor_handle, functor_id, functor_config);

      const mygsl::i_unary_function & functor = functor_handle.get();
      double dx = 0.05;
      std::cout << "# polynomial:" << std::endl;
      for (double x = 0.0; x <= 5.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << functor(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

    {
      functor_id = "mygsl::plain_function_wrapper";
      functor_config.store_string("function", "cos");
      mygsl::unary_function_handle_type functor_handle;
      ff.create(functor_handle, functor_id, functor_config);

      const mygsl::i_unary_function & functor = functor_handle.get();
      double dx = 0.05;
      std::cout << "# cos:" << std::endl;
      for (double x = 0.0; x <= 5.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << functor(x) << std::endl;
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
