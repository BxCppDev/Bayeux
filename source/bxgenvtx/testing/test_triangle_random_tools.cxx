// -*- mode: c++ ; -*- 
// test_triangle_random_tools.cxx
 
#include <cstdlib>
#include <fstream>
#include <string>
#include <exception>

#include <genvtx/genvtx_config.h>
#include <genvtx/triangle_random_tools.h>
#include <geomtools/utils.h>
#include <geomtools/polyline_3d.h>

#include <geomtools/geomtools_config.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'genvtx' program library." << endl; 
  
      bool debug = false;
      bool verbose = false;
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
               else if ((option == "-v") || (option == "--verbose")) 
                 {
                   verbose = true;
                 }
               else if ((option == "-D") || (option == "--draw")) 
                 {
                   draw = true;
                 }
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

      std::string       rng_id = "mt19937";
      unsigned long int rng_seed = 12345;
      mygsl::rng random (rng_id, rng_seed);

      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", "test_trt_");

      geomtools::vector_3d A(1,2.5,0), B(-1,1,2.5), C(-1,-2.0,0.5);
      int nshoots = 10000;
      for (int i = 0; i < nshoots; i++)
        {
          geomtools::vector_3d vertex
            = genvtx::triangle_random_surface (A, B, C, random);
          geomtools::gnuplot_draw::basic_draw_point (tmp_file.out (), 
                                                     vertex, 
                                                     true);
        }
      tmp_file.out () << endl << endl;
      geomtools::basic_polyline_3d ABCA;
      ABCA.push_back(A);
      ABCA.push_back(B);
      ABCA.push_back(C);
      ABCA.push_back(A);
      geomtools::gnuplot_draw::basic_draw_polyline (tmp_file.out (),ABCA);
      tmp_file.out () << endl << endl;

      if (draw)
        {         
          Gnuplot g1 ("dots"); 
          { 
            g1.cmd ("set grid");
            g1.cmd ("set title 'test triangle_random_tools'");
            g1.cmd ("set size ratio -1");
            g1.cmd ("set view equal xyz");
            g1.cmd ("set xyplane at -3");
            g1.set_xrange (-3, +3).set_yrange (-3, +3).set_zrange (-3, +3);
            g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
            std::ostringstream plot_cmd;
            plot_cmd << "splot '" << tmp_file.get_filename () 
                     << "' index 0 title 'Random vertices' with dots , ";
            plot_cmd << " '" << tmp_file.get_filename () 
                     << "' index 1 title 'Facet' with lines lt 3 lw 1" ;
            g1.cmd (plot_cmd.str ());

            g1.showonscreen (); // window output
            geomtools::gnuplot_drawer::wait_for_key ();
            usleep (200);

          }
        }

  
      clog << "The end." << endl;
    }
  catch (exception & x)
    { 
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_triangle_random_tools.cxx
