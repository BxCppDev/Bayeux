// -*- mode: c++; -*- 
// test_dummy_genbb_help.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/dummy_genbb_help.h>

int
main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
  {

    std::cerr << "Hello, this is a sample program for class 'dummy_genbb_help'!" << std::endl; 
  
    bool debug = false;

    int iarg = 1;
    while (iarg < argc_)
    {
    
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      // <here you may add more switches...>

      iarg++;
    }
    
    genbb::dummy_genbb_help my_dummy_genbb_help;
    // <here you may put stuff for testing the class>

  }
  catch (std::exception & x)
  {
    std::cerr << "error: " << x.what() << std::endl; 
    error_code=EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "error: " << "unexpected error!" << std::endl; 
    error_code=EXIT_FAILURE;
  }
  return error_code;
}

// end of test_dummy_genbb_help.cxx
