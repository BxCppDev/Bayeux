// -*- mode: c++ ; -*-
// test_polycone_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/polycone.h>
#include <geomtools/gnuplot_draw.h>
#include <datatools/utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'polycone'!" << endl;

      bool debug = false;
      string datafile;
      double zmin = std::numeric_limits<double>::quiet_NaN ();
      double zmax = std::numeric_limits<double>::quiet_NaN ();

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
               else
                 {
                    clog << "warning: ignoring option '" << option << "'!" << endl;
                 }
            }
          else
            {
              string argument = token;
              if (datafile.empty ())
                {
                  datafile = argument;
                }
              else
                {
                  clog << "warning: ignoring argument '" << argument << "'!" << endl;
                }
            }
          iarg++;
      }

      geomtools::polycone PC1;
      if (datafile.empty ())
        {
          datafile = "${GEOMTOOLS_DATA_DIR}/testing/data/test_hamamatsu_R5912MOD_1.data";
        }
      datatools::fetch_path_with_env (datafile);
      zmin = -100 * CLHEP::mm;
      //zmin = -54 * CLHEP::mm;
      PC1.initialize (datafile, zmin, zmax);
      clog << "Polycone: " << PC1 << endl;

      geomtools::polycone PC2;
      zmax = zmin;
      datatools::invalidate (zmin);
      PC2.initialize (datafile, zmin, zmax);
      clog << "Polycone: " << PC2 << endl;

      {
        geomtools::vector_3d polycone_pos;
        geomtools::rotation polycone_rot;
        geomtools::create_rotation (polycone_rot, 0.0, 0.0, 0.0);
        geomtools::gnuplot_draw::draw_polycone (cout,
                                                polycone_pos,
                                                polycone_rot,
                                                PC1);
        cout << endl << endl;
      }

      {
        geomtools::vector_3d polycone_pos;
        geomtools::rotation polycone_rot;
        geomtools::create_rotation (polycone_rot, 0.0, 0.0, 0.0);
        geomtools::gnuplot_draw::draw_polycone (cout,
                                                polycone_pos,
                                                polycone_rot,
                                                PC2);
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

// end of test_polycone_2.cxx
