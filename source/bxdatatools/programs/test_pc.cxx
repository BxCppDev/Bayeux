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

int 
main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      std::cerr << "Hello, this is a sample program for class 'properties_commander'!" << std::endl; 
      bool debug=false;

      int iarg = 1;
      while (iarg < argc_) 
	{
    
	  std::string arg = argv_[iarg];

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
	std::string shell_name = "test_pc";

	if (debug) std::clog << "starting '" << shell_name 
			     << "' shell..." << std::endl;
	std::string shell_prompt = "commander> "; //shell_name + "> ";
	std::string command_str;

	bool stop = false;
	do 
	  {
	    command_str.clear ();
	    std::clog << shell_prompt;
	    if (! std::getline (std::cin, command_str))
	      {
		stop = true;
		break;
	      }
	    if (command_str.empty ()) continue;
	    du::command the_command;
	    the_command.init (command_str);
	    if (du::command::code_is_error (the_pc.run (the_command)))
	      {
		std::string msg = the_command.get_error_message ();
		if (msg.empty ()) msg = "<no message>";
		std::cerr << shell_name << ": error: " 
			  << msg
			  << std::endl;
	      }
	    else
	      {
		if (! the_command.get_returns ().empty ())
		  {
		    std::cout << the_command.get_returns () << std::endl;
		  }
		if (debug) the_prop.tree_dump (std::clog, "Current status");
	      }
	  } while (! stop);
	if (command_str.empty ()) std::clog << std::endl;
	if (debug) std::clog << "'test_pc' shell stopped." << std::endl;
      }

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_pc.cxx
