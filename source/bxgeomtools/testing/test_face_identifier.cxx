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

    geomtools::face_identifier fid2;
    fid2.set_face_bit(0x4);
    std::clog << "FID = " << fid2.to_string() << std::endl;

    geomtools::face_identifier fid3;
    fid3.append_part(1);
    fid3.append_part(0);
    fid3.set_face_bits(0x16);
    std::clog << "FID = " << fid3.to_string() << std::endl;

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
