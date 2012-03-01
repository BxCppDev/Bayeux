// -*- mode: c++ ; -*- 
// test_logical_volume.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/box.h>
#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'logical_volume' !" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;

          iarg++;
      }

      if (debug) clog << "DEBUG: Creating 'world_box'..." << endl;
      geomtools::box world_box (1.0, 1.0, 1.0);
      if (debug) clog << "DEBUG: 'world_box' done." << endl;
      geomtools::logical_volume world_log ("world", world_box);
      world_log.parameters ().store ("material", "vacuum");
      world_log.parameters ().store ("visualization.visible", false);
      world_log.tree_dump (cout, "World");

      geomtools::box vessel_box (0.9, 0.9, 0.9);
      geomtools::logical_volume vessel_log ("vessel", vessel_box);
      vessel_log.parameters ().store ("material", "copper");
      vessel_log.parameters ().store ("visualization.visible", true);  
      vessel_log.parameters ().store ("visualization.color", "red");  
      vessel_log.tree_dump (cout, "Vessel");

      geomtools::box gas_box (0.85, 0.85, 0.85);
      geomtools::logical_volume gas_log ("gas", gas_box);
      gas_log.parameters ().store ("material", "air");
      gas_log.parameters ().store ("visualization.visible", true);  
      gas_log.parameters ().store ("visualization.color", "blue");  
      gas_log.tree_dump (cout, "Air");
 
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
  return error_code;
}

// end of test_logical_volume.cxx
