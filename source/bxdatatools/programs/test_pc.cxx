// -*- mode: c++; -*- 
// test_pc.cxx
// Test program for the 'properties_commander' class.
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <datatools/utils/properties.h>
#include <datatools/utils/properties_commander.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'properties_commander' class..." << endl; 
      bool debug=false;

      int iarg = 1;
      while (iarg < argc_) 
	{
    
	  string arg = argv_[iarg];

	  if ((arg == "-d") || (arg == "--debug")) debug = true;

	  iarg++;
	}
    
      namespace du = datatools::utils;
      
      // test bloc:
      {

	// the 'properties' object to be managed by the 'commander':
	du::properties the_prop;
	
	// the 'commander' object:
	du::properties_commander the_pc (debug);
	the_pc.add_target ("dummy", the_prop);

	// this allows to create/remove 'properties' objects
	// managed the 'commander' via the 'run' protocol:
	the_pc.set_allows_map_commands (true);

	// an interactive shell:
	string shell_name = "test_pc";

	if (debug) clog << "starting '" << shell_name 
			<< "' shell..." << endl;
	string shell_prompt = "commander> "; //shell_name + "> ";
	string command_str;

	bool stop = false;
	do 
	  {
	    command_str.clear ();
	    clog << shell_prompt;
	    if (! getline (cin, command_str))
	      {
		stop = true;
		break;
	      }
	    if (command_str.empty ()) continue;
	    du::command the_command;
	    the_command.init (command_str);
	    if (du::command::code_is_error (the_pc.run (the_command)))
	      {
		string msg = the_command.get_error_message ();
		if (msg.empty ()) msg = "<no message>";
		clog << shell_name << ": error: " 
		     << msg
		     << endl;
	      }
	    else
	      {
		if (! the_command.get_returns ().empty ())
		  {
		    cout << the_command.get_returns () << endl;
		  }
		if (debug) the_prop.tree_dump (clog, "Current status");
	      }
	  } while (! stop);
	if (command_str.empty ()) clog << endl;
	if (debug) clog << "'test_pc' shell stopped." << endl;
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
  return error_code;
}

// end of test_pc.cxx
