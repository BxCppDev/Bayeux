// -*- mode: c++ ; -*- 
// test_manager.cxx
 
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <list>

#include <datatools/utils/utils.h>
#include <datatools/utils/multi_properties.h>

#include <materials/manager.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'manager' !" << endl; 
  
      bool debug = false;
      list<string> input_files;
      bool generate_gdml = false;

      int iarg = 1;
      bool parsing_options = true;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
	      if (! parsing_options)
		{
		  clog << "warning: ignoring any option after the list of arguments !" << endl; 
		  break;
		}
	      string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else if ((option == "-g") || (option == "--generate-gdml")) 
                 {
                   generate_gdml = true;
                 }
               else 
                 { 
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
	      parsing_options = false;
              string argument = token; 
              input_files.push_back (argument);
            }
          iarg++;
      }

      mat::manager my_manager;
      my_manager.set_debug (debug);
      for (list<string>::iterator i = input_files.begin ();
	    i != input_files.end ();
	   i++)
	{
	  datatools::utils::multi_properties config ("name", "type");
	  datatools::utils::fetch_path_with_env (*i);
	  config.read (*i);
	  if (debug)
	    {
	      clog << "Config file '" << *i << "' : " << endl;
	      config.tree_dump (clog, "", "DEBUG: ");
	    }
	  my_manager.load (config);
	}
      my_manager.tree_dump (clog, "Material manager: ");

      if (generate_gdml)
	{
	  my_manager.export_gdml (cout);
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

// end of test_manager.cxx
