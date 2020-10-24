// test_manager.cxx

// Ourselves:
#include <materials/manager.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <list>

// Third Party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/multi_properties.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'materials::manager' !" << endl;
    bool debug = false;
    list<string> input_files;
    bool aao = false;
    int iarg = 1;
    bool parsing_options = true;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        if (! parsing_options) {
          clog << "WARNING: ignoring any option after the list of arguments !" << endl;
          break;
        }
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
            debug = true;
        } else if ((option == "-aao") || (option == "--alias-allow-overload")) {
          aao  = true;
        } else {
          clog << "WARNING: ignoring option '" << option << "'!" << endl;
        }
      } else {
        parsing_options = false;
        string argument = token;
        input_files.push_back (argument);
      }
      iarg++;
    }

    materials::manager MatMgr;
    MatMgr.set_alias_allow_overload(aao);
    if (debug) clog << "DEBUG: alias_allow_overload = "
                    << MatMgr.is_alias_allow_overload() << "" << endl;
    MatMgr.set_debug (debug);
    for (list<string>::iterator i = input_files.begin ();
         i != input_files.end ();
         i++) {
      datatools::multi_properties config ("name", "type");
      datatools::fetch_path_with_env (*i);
      datatools::read_config(*i, config);
      if (debug) {
        clog << "Config file '" << *i << "' : " << endl;
        config.tree_dump (clog, "", "DEBUG: ");
      }
      MatMgr.load (config);
    }
    MatMgr.tree_dump (clog, "Material manager: ");
    {
      datatools::object_configuration_description OCD;
      if ( datatools::load_ocd<materials::manager>(OCD)) {
        OCD.print(std::clog, "*** ");
        OCD.dump(std::clog, "OCD: ");
        std::ofstream fscf ("test_OCD_materials_manager.sample.conf");
        OCD.generate_sample_configuration(fscf, "the configuration of a 'materials::manager' object");
      } else {
        std::cerr << "ERROR: Cannot find OCD support for the 'materials::manager' class."
                  << std::endl;
      }
    }

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
