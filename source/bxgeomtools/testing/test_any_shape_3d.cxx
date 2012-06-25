// -*- mode: c++ ; -*- 
// test_any_shape_3d.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/any_shape_3d.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'any_shape_3d'!" << endl; 
  
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
    
      geomtools::any_shape_3d my_shape ("bidule");
      my_shape.properties ().store ("length", 3.2 *CLHEP::cm);
      my_shape.properties ().store ("width", 1.1 * CLHEP::cm);
      my_shape.properties ().store ("thickness", 1.76 * CLHEP::mm);
      my_shape.properties ().store ("hole_diameter", 5.4 * CLHEP::mm);
      my_shape.properties ().store ("number_of_holes", 12);
      my_shape.tree_dump (clog, "my_shape");

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

// end of test_rectangle.cxx
