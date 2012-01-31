// -*- mode: c++ ; -*- 
// test_polyhedra.cxx 

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/polyhedra.h>
#include <geomtools/gnuplot_draw.h>
#include <datatools/utils/utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'polyhedra'!" << endl; 
  
      bool debug = false;
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
               else if ((option == "-D") || (option == "--draw")) 
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
    
      geomtools::polyhedra my_polyhedra;
      string datafile = "${GEOMTOOLS_DATA_DIR}/testing/data/test_light_guide_0.data";
      datatools::utils::fetch_path_with_env (datafile);
      my_polyhedra.initialize (datafile);
      if (debug)
	{
	  my_polyhedra.tree_dump (clog,"Polyhedra: ", "DEBUG: ");
	}
      clog << "Polyhedra: " << my_polyhedra << endl;

      if (draw)
      {
	geomtools::vector_3d polyhedra_pos;
	geomtools::rotation polyhedra_rot;
	geomtools::create_rotation (polyhedra_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_polyhedra (cout, 
						 polyhedra_pos, 
						 polyhedra_rot, 
						 my_polyhedra);
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

// end of test_polyhedra.cxx
