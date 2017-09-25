// test_timestamp.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <iomanip>

// - Bayeux/datatools:
#include <datatools/clhep_units.h>

// This project:
#include <events/timestamp.h>
#include <events/timestamp.ipp>

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {

    long int seed = 314159;
    srand48(seed);

    events::timestamp tref;
    tref.set_seconds(1506290000);

    events::timestamp ts;
    ts.set_seconds(1506290786);
    ts.set_microseconds(123);
    ts.tree_dump(std::clog, "Timestamp: ");
    std::clog << "Timestamp           = " << ts << std::endl;
    std::clog << "as a real from tref = " << std::setprecision(15)
              << ts.to_real(tref.get_seconds()) / CLHEP::second << " s" << std::endl;

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
