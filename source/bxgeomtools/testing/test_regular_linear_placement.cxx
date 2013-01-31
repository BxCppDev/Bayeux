// -*- mode: c++; -*- 
// test_regular_linear_placement.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unistd.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/regular_linear_placement.h>
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#endif

#include <datatools/temporary_files.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      bool draw = false;
      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token; 
              if ((option == "-d") || (option == "--debug")) 
                {
                  debug = true;
                }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
             else if (option == "--no-draw") 
                {
                  draw = false;
                }
             else if (option == "--draw") 
                {
                  draw = true;
                }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
              else 
                { 
                  clog << "warning: ignoring option '" << option << "'!" << endl; 
                }
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
        }
 
      geomtools::vector_3d  basic_position (1., 2., 3.);
      cout << "Basic position : " << basic_position << endl;

      double phi   = -30.0 * CLHEP::degree;
      double theta = 75.0 * CLHEP::degree;
      double delta = 0.0 * CLHEP::degree;
 
      geomtools::placement basic_placement (basic_position, phi, theta, delta);

      geomtools::vector_3d step (+1, +0.5, 0.0);


      geomtools::regular_linear_placement rlp (basic_placement, step, 4);
      rlp.tree_dump (clog, "regular_linear_placement", ">>> ");

      for (int i = 0; i < rlp.get_number_of_items (); i++)
        {
          ostringstream title;
          title << "Item #" << i << " placement:";
          geomtools::placement pi = rlp.i_placement::get_placement (i);
          pi.tree_dump (clog, title.str (), ">>> ");
        }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      if (draw) {
        datatools::temp_file tmp_file;
        tmp_file.set_remove_at_destroy (true);
        tmp_file.create ("/tmp", ".tmp_drawer_");
        
        geomtools::box b (3.0, 1.0, 0.3);
        for (int i = 0; i < rlp.get_number_of_items (); i++)
          {
            geomtools::gnuplot_draw::draw (tmp_file.out (),
                                           rlp.i_placement::get_placement (i),
                                           b);
          }
        tmp_file.close ();
        Gnuplot g1 ("lines");
        g1.set_title ("test_regular_linear_placement");
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
        g1.showonscreen (); 
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what() << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_regular_linear_placement.cxx
