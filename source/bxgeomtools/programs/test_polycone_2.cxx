// -*- mode: c++ ; -*- 
// test_polycone.cxx 

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/polycone.h>
#include <geomtools/gnuplot_draw.h>
#include <datatools/utils/utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'polycone'!" << endl; 
  
      bool debug = false;
      string datafile;

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
    	
      geomtools::polycone my_polycone;
      if (datafile.empty ())
	{
	  datafile = "${GEOMTOOLS_ROOT}/resources/data/test_hamamatsu_R5912MOD_1.data";
	}
      datatools::utils::fetch_path_with_env (datafile);
      my_polycone.initialize (datafile);
      clog << "Polycone: " << my_polycone << endl;

      {
	geomtools::vector_3d polycone_pos;
	geomtools::rotation polycone_rot;
	geomtools::create_rotation (polycone_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_polycone (cout, 
						polycone_pos, 
						polycone_rot, 
						my_polycone);
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

// end of test_polycone.cxx
