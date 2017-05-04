// test_mcnp_surface.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/clhep_units.h>

// This project:
#include <mctools/mcnp/geometry/surface.h>
#include <mctools/mcnp/format.h>

int main(int /* argc_ */, char ** /* argv_ */)
{
  namespace m2g = mctools::mcnp::geometry;
  int error_code = EXIT_SUCCESS;
  try {

    std::clog << "Test program for class 'mctools::mcnp::geometry::surface'!" << std::endl;

    {
      m2g::surface plane;
      plane.set_name("P");
      plane.set_id(6);
      plane.set_type(m2g::SURF_PLANE_X);
      plane.add_parameter("D", 16.0 * CLHEP::mm, "length");
      plane.print_card(std::clog, mctools::mcnp::format::CARD_COMMENT);
    }

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
