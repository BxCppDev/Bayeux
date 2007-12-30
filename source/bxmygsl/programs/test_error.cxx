// test_error.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/error.h>

int main( int argc_ , char ** argv_ )
{
  try {

    mygsl::error::off();

    std::clog << "run test error 1..." << std::endl;
    MYGSL_ERROR("test error 1",1);
    gsl_error("test error 1",__FILE__, __LINE__,1);


    mygsl::error::on();

    std::clog << "run test error 2..." << std::endl;
    MYGSL_ERROR("test error 2",2);

  }
  catch( std::exception & x ) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// end of test_error.cxx
