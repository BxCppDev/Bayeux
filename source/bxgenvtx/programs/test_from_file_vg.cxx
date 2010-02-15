// -*- mode: c++ ; -*- 
// test_from_file_vg.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genvtx/from_file_vg.h>
#include <mygsl/rng.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'from_file_vg'!" << endl; 
  
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

      std::string       rng_id = "mt19937";
      unsigned long int rng_seed = 12345;
      mygsl::rng random (rng_id, rng_seed);
     
      genvtx::from_file_vg vg ("${GENVTX_ROOT}/resources/vertices.data");
    
      geomtools::vector_3d vertex;
           
      while (true)
	{
	  vg.shoot_vertex (random, vertex);
	  if (! geomtools::is_valid (vertex))
	    {
	      clog << "End of vertex source file." << endl;
	      break;
	    }
	  cout << vertex << endl;
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

// end of test_from_file_vg.cxx
