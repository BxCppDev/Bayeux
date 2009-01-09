// -*- mode: c++; -*- 
// test_placement.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/placement.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      geomtools::vector_3d pos;
      pos.set (1., 2., 3.);
      std::cout << "Vector: " << pos << std::endl;

      geomtools::placement p (pos, 0.0, 90. * CLHEP::degree, 0.0);
      p.dump ();

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what() << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_placement.cxx
