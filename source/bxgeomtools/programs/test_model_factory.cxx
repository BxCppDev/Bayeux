// -*- mode: c++ ; -*- 
// test_model_factory.cxx 

#include <cstdlib> 
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/utils.h>
#include <geomtools/model_factory.h>
#include <geomtools/drawer.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'model_factory'!" << endl; 
  
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
    
      geomtools::i_model::g_devel = debug;

      geomtools::model_factory factory;
      factory.set_debug (debug);
      geomtools::i_model::g_devel = debug;

      string setup_filename = "${GEOMTOOLS_ROOT}/resources/test/test_geometry_models.setup"; 
      datatools::utils::fetch_path_with_env (setup_filename);
      factory.load (setup_filename);
      factory.lock ();
      factory.tree_dump (clog);

      datatools::utils::properties drawer_config;
      geomtools::placement p;
      //p.set (0,0,0,30,27,12);
      //p.set (0, 0, 0, 30 * CLHEP::degree, 45 * CLHEP::degree, 0);
      p.set (0, 0, 0, 30 * CLHEP::degree, 0, 0);

      p.tree_dump (clog, "Placement");

      geomtools::drawer::draw (factory, "box3", p, drawer_config);

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

// end of test_model_factory.cxx
