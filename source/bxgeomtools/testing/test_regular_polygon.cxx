// -*- mode: c++ ; -*-
// test_regular_polygon.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/regular_polygon.h>
#include <geomtools/gnuplot_draw.h>

using namespace std;

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'regular_polygon'!" << endl;

      // bool debug = false;

      // int iarg = 1;
      // while (iarg < argc_)
      //   {
      //     string token = argv_[iarg];

      //     if (token[0] == '-')
      //       {
      //          string option = token;
      //          if ((option == "-d") || (option == "--debug"))
      //            {
      //              debug = true;
      //            }
      //          else
      //            {
      //               clog << "warning: ignoring option '" << option << "'!" << endl;
      //            }
      //       }
      //     else
      //       {
      //         string argument = token;
      //         {
      //           clog << "warning: ignoring argument '" << argument << "'!" << endl;
      //         }
      //       }
      //     iarg++;
      // }

      geomtools::regular_polygon my_regular_polygon;
      size_t n_sides = 6;
      double radius = 1.25;
      my_regular_polygon.set_n_sides (n_sides);
      my_regular_polygon.set_r (radius);
      my_regular_polygon.tree_dump (clog, "my_regular_polygon");

      {
        geomtools::vector_3d regular_polygon_pos;
        geomtools::rotation_3d regular_polygon_rot;
        geomtools::create_rotation (regular_polygon_rot, 0.0, 0.0, 0.0);
        geomtools::gnuplot_draw::draw_regular_polygon (cout,
                                                       regular_polygon_pos,
                                                       regular_polygon_rot,
                                                       my_regular_polygon);
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
  return (error_code);
}
