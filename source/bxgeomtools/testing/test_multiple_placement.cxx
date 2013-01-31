// -*- mode: c++; -*- 
// test_multiple_placement.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unistd.h>

#include <datatools/temporary_files.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/multiple_placement.h>
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      bool rotate = false;
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
               else if (option == "-r") 
                 {
                   rotate = true;
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
#endif
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

      geomtools::multiple_placement mp;

      size_t sz = 20;
      for (int i = 0; i < sz; i++)
        {
          double x = 3.0 + 0.5 * i;
          double y = 1.5 * sin (x - 2);
          double z = 1.0 + i * 0.2;
          geomtools::vector_3d  position (x, y, z);
          double phi   = 0.0 * CLHEP::degree;
          double theta = 0.0 * CLHEP::degree;
          double delta = 0.0 * CLHEP::degree;
          if (rotate)
            {
              phi   = i * 5.0 * CLHEP::degree;
              theta = i * 3.0 * CLHEP::degree;
              delta = i * 10.0 * CLHEP::degree;
            }
          geomtools::placement p (position, phi, theta, delta);
          mp.add (p);
        }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      if (draw)
        {
          datatools::temp_file tmp_file;
          tmp_file.set_remove_at_destroy (true);
          tmp_file.create ("./", "tmp_drawer_");        
          geomtools::box b (0.3, 0.2, 0.1);
          for (int i = 0; i < mp.get_number_of_items (); i++)
            {
              geomtools::gnuplot_draw::draw (tmp_file.out (),
                                             mp.i_placement::get_placement (i),
                                             b);
            }
          tmp_file.out ().close ();
          Gnuplot g1 ("lines");
          //g1.set_xrange (-5,+10);
          //g1.set_yrange (-2,+8);
          //g1.set_zrange (0,+2);
          g1.set_title ("test_multiple_placement");
          g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
          g1.plotfile_xyz (tmp_file.get_filename () , 1, 2, 3, "3D view");
          g1.showonscreen (); 
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }
#endif
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

// end of test_multiple_placement.cxx
