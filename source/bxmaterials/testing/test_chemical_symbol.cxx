// test_chemical_symbol.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project
#include <materials/chemical_symbol.h>

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
      std::clog << "Test program for class 'chemical_symbol' !" << std::endl;

      for (int i = 0; i < (int) materials::chemical_symbol::nb_chemical_symbols(); i++) {
        std::clog << i << " " << materials::chemical_symbol::z_is_tabulated(i) << std::endl;
      }

      std::string symbol = "H";
      std::clog << "Symbol='" << symbol << "' Z=" << materials::chemical_symbol::z_from_symbol (symbol)  << std::endl;

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
