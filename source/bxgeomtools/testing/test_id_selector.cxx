// -*- mode: c++ ; -*- 
// test_id_selector.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/id_mgr.h>
#include <geomtools/id_selector.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'id_selector'!" << endl; 
  
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
    
      geomtools::id_mgr my_id_mgr;
      my_id_mgr.load ("${GEOMTOOLS_DATA_DIR}/testing/config/test_geom_id_mgr.lis");
      if (debug) my_id_mgr.tree_dump (clog, "ID manager:", "DEBUG: ");

      geomtools::id_selector IDS (my_id_mgr);

      string selector_rules = "category=source_pad module={0;1} layer={*} strip={1;2;4;5;10} pad=[3;7]";
      IDS.initialize (selector_rules);
      if (debug) IDS.dump (clog, "ID selector: ");

      for (int i = 0; i < 20; i++)
        {
          // Randomize a geometry ID:
          uint32_t module = (uint32_t) (drand48 () * 2);
          uint32_t layer = (uint32_t) (drand48 () * 10);
          uint32_t strip = (uint32_t) (drand48 () * 12);
          uint32_t pad   = (uint32_t) (drand48 () * 8);
          geomtools::geom_id gid;
          my_id_mgr.make_id ("source_pad", gid);
          gid.set_address (module, layer, strip, pad);
          
          // Check it:
          if (IDS.match (gid))
            {
              clog << "ID=" << gid << " matches the selector rules !" << endl;
            }
          else
            {
              clog << "ID=" << gid << " does not match the selector rules !" << endl;
            }
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

// end of test_id_mgr.cxx
