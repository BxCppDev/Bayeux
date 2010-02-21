// -*- mode: c++ ; -*- 
// test_circle.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/circle.h>
#include <geomtools/gnuplot_draw.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'circle'!" << endl; 
  
      bool debug = false;

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
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
      geomtools::circle my_circle;
      double radius = 1.0;
      my_circle.set_r (radius);
      my_circle.tree_dump (clog, "my_circle");

      {
	geomtools::vector_3d circle_pos;
	geomtools::rotation_3d circle_rot;
	geomtools::create_rotation (circle_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_circle (cout, 
					      circle_pos,
					      circle_rot, 
					      my_circle);
  
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

// end of test_circle.cxx
