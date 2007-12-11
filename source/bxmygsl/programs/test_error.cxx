// test_error.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/error.h>

int main( int argc_ , char ** argv_ )
{
  try {

    GSL_ERROR("test error 1",1);

    mygsl::error::off();

    GSL_ERROR("test error 2",1);

  }
  catch( std::exception & x ) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// end of test_error.cxx
