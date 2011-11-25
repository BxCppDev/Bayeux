// -*- mode: c++ ; -*- 
// domestic_1.cxx

#include <cstdlib>
#include <iostream>
#include <exception>

#include <geomtools/id_mgr.h>

int main (void)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      // Declare a GID manager :
      geomtools::id_mgr the_gid_manager;

      // Load the configuration file for categories :
      the_gid_manager.load ("domestic_categories.lis");

      // Print the GID manager's internal lookup table :
      the_gid_manager.tree_dump (std::clog);

    }
  catch (exception & x)
    {
      std::cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
   return error_code;
}

// end of domestic_1.cxx
