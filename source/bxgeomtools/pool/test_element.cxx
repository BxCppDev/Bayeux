// -*- mode: c++; -*- 
// test_element.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <geomtools/element.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;

      geomtools::element::load_table_of_elements ();
      geomtools::element::print_table_of_elements (std::cout);

      geomtools::element::create ("Devilium", 
				  "Dv", 
				  666.00 * CLHEP::gram / CLHEP::mole,
				  666);

      geomtools::element::get ("Hydrogen")->tree_dump (std::cout, 
						      "A well known element:");
      geomtools::element::get ("Devilium")->tree_dump (std::cout, 
						      "An evil element:");

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_element.cxx
