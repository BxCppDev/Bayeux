// -*- mode: c++; -*-
// test_sphere_2.cxx
// gnuplot macro: ./tests/test_sphere.gpl

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/sphere.h>
#include <geomtools/gnuplot_draw.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      bool debug = false;
      long seed  = 314159;
      bool test1 = true;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;
          if (arg == "-t1" || arg == "--test1") test1 = false;

          iarg++;
        }

      srand48 (seed);

      // test 1:
      if (test1)
        {
          geomtools::sphere my_sphere (1.8 * CLHEP::mm, 3.0 * CLHEP::mm);
          my_sphere.set_phi(20.0 * CLHEP::degree, 70.0 * CLHEP::degree );
          my_sphere.set_theta(30.0 * CLHEP::degree, 40.0 * CLHEP::degree );
          clog << "test 2: Sphere #2   = " << my_sphere << " " << endl;

          geomtools::vector_3d sphere_pos;
          geomtools::rotation sphere_rot;
          geomtools::create_rotation (sphere_rot, 0.0, 0.0, 0.0);
          geomtools::gnuplot_draw::draw_sphere (cout,
                                                sphere_pos,
                                                sphere_rot,
                                                my_sphere);
          cout << endl << endl;
       }
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}
