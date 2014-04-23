// -*- mode: c++ ; -*-
// test_cone.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/geomtools_config.h>
#include <geomtools/cone.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_draw.h>
#endif
using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'cone'!" << endl;

    // bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        // string option = token;
        // if ((option == "-d") || (option == "--debug")) {
        //   debug = true;
        // } else {
        //   clog << "warning: ignoring option '" << option << "'!" << endl;
        // }
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    double z1 = 4.0;
    double z2 = 2.0;
    double r1 = 2.0;
    double r2 = 3.0;
    double surface = geomtools::cone::compute_frustrum_lateral_surface(z1, z2, r1, r2);
    std::clog << "Conical frustrum surface : " << surface << std::endl;
    double volume = geomtools::cone::compute_frustrum_volume(z1, z2, r1, r2);
    std::clog << "Conical frustrum volume  : " << volume << std::endl;

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    {
      geomtools::vector_3d circle_pos (0, 0, z1);
      geomtools::rotation_3d circle_rot;
      geomtools::create_rotation (circle_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_circle (cout,
                                            circle_pos,
                                            circle_rot,
                                            r1);
      circle_pos.set(0, 0, z2);
      geomtools::gnuplot_draw::draw_circle (cout,
                                            circle_pos,
                                            circle_rot,
                                            r2);

    }
#endif

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_circle.cxx
