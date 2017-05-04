// test_mcnp_mcsetup.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// // - Bayeux/mygsl:
// #include <mygsl/rng.h>

// This project:
#include <mctools/mcnp/mcsetup.h>

int main(int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {

    std::clog << "Test program for class 'mctools::mcnp::mcsetup'!" << std::endl;

    mctools::mcnp::mcsetup mcs;
    mcs.set_title("Simulation");

  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
