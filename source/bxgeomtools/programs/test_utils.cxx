// -*- mode: c++; -*- 
// test_utils.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      geomtools::vector_3d pos;
      pos.set (1., 2., 3.);
      std::cout << "Vector: " << pos << std::endl;

      geomtools::rotation rot;
      geomtools::create_rotation(rot, 0.0, 90. * CLHEP::degree, 0.0);
      geomtools::tree_dump (rot, std::cout, "Rotation matrix:");

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

// end of test_utils.cxx
