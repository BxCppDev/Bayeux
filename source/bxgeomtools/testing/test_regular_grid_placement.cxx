// -*- mode: c++; -*- 
// test_regular_grid_placement.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unistd.h>

#include <geomtools/regular_grid_placement.h>
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>

#include <datatools/utils/temporary_files.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      bool centered = geomtools::regular_grid_placement::UNCENTERED;
      bool rotate = false;
      bool draw = true;
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
              else if (option == "-c") 
                {
                  centered = geomtools::regular_grid_placement::CENTERED;
                }
              else if (option == "-r") 
                {
                  rotate = true;
                }
              else if (option == "--no-draw") 
                {
                  draw = false;
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

      geomtools::vector_3d  basic_position (1., 2., 1.);
      cout << "Basic position : " << basic_position << endl;

      double phi   = 0.0 * CLHEP::degree;
      double theta = 0.0 * CLHEP::degree;
      double delta = 0.0 * CLHEP::degree;
      if (rotate)
        {
          phi   = 30.0 * CLHEP::degree;
          theta = 75.0 * CLHEP::degree;
          delta = 20.0 * CLHEP::degree;
        }

      geomtools::placement basic_placement (basic_position, phi, theta, delta);

      geomtools::regular_grid_placement rxygp (basic_placement, 
                                               +2, +1, 
                                               5, 3, 
                                               geomtools::regular_grid_placement::MODE_YZ, 
                                               centered);
      rxygp.tree_dump (clog, "regular_grid_placement", ">>> ");

      for (int i = 0; i < rxygp.get_number_of_items (); i++)
        {
          ostringstream title;
          title << "Item #" << i << " placement:";
          geomtools::placement pi = rxygp.i_placement::get_placement (i);
          pi.tree_dump (clog, title.str (), ">>> ");
        }

      if (draw)
        {
          datatools::utils::temp_file tmp_file;
          tmp_file.set_remove_at_destroy (true);
          tmp_file.create ("/tmp", ".tmp_drawer_");
        
          geomtools::box b (0.25, 1.9, 0.9);
          clog << "number_of_columns  = " << rxygp.get_number_of_columns () << endl;
          clog << "number_of_rows     = " << rxygp.get_number_of_rows () << endl;
          for (int i = 0; i < rxygp.get_number_of_items (); i++)
            {
              geomtools::gnuplot_draw::draw (tmp_file.out (),
                                             rxygp.i_placement::get_placement (i),
                                             b);
            }
          tmp_file.close ();
          Gnuplot g1 ("lines");
          g1.set_xrange (-5,+15);
          g1.set_yrange (-5,+15);
          g1.set_zrange (-5,+10);
          g1.set_title ("test_regular_grid_placement");
          g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
          g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
          g1.showonscreen (); 
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }

      if (draw)
        {
          datatools::utils::temp_file tmp_file;
          tmp_file.set_remove_at_destroy (true);
          tmp_file.create ("/tmp", ".tmp_drawer_");
        
          geomtools::box b (0.3, 0.2, 0.1);
          {
            int column = 0;
            int row   = 0;
            geomtools::gnuplot_draw::draw (tmp_file.out (),
                                           rxygp.get_placement (column, row),
                                           b);
          }
          {
            int column = 2;
            int row   = 1;
            geomtools::gnuplot_draw::draw (tmp_file.out (),
                                           rxygp.get_placement (column, row),
                                           b);
          }
          {
            int column = 3;
            int row   = 3;
            geomtools::gnuplot_draw::draw (tmp_file.out (),
                                           rxygp.get_placement (column, row),
                                           b);
          }
          tmp_file.close ();
          Gnuplot g1 ("lines");
          g1.set_xrange (-5,+10);
          g1.set_yrange (-2,+8);
          g1.set_zrange (0,+2);
          g1.set_title ("test_regular_grid_placement");
          g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
          g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
          g1.showonscreen (); 
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }
      
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

// end of test_regular_grid_placement.cxx
