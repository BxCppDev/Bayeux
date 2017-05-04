// test_face_identifier.cxx

// Ourselves:
#include <geomtools/face_identifier.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <geomtools/color.h>

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Hello, this is a sample program for class 'geomtools::face_identifier'!" << std::endl;

    geomtools::face_identifier fid;
    fid.set_face_index(12);
    std::clog << "FID = " << fid.to_string() << std::endl;

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
