// -*- mode: c++; -*- 
// test_command.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <datatools/utils/command.h>
#include <datatools/utils/i_invokable.h>

using namespace std;

struct test : public datatools::utils::i_invokable
{
  bool debug;
  
public:
  // ctor:
  test (bool debug_ = false)
  {
    debug = debug_;
  }
  
  void 
  do_invoke (datatools::utils::command & command_)
  {
    command_.reset_output ();
      
    string command_name        = command_.get_name ();
    size_t      number_of_arguments = command_.get_number_of_arguments ();
    size_t      number_of_options   = command_.get_number_of_options ();
      
    if (number_of_options > 0)
      {
	string option  = command_.get_option (0);
	if ((option == "-d") || (option == "--debug"))
	  {
	    clog << "switching on debug mode." << endl;
	  }
	else if ((option == "-D") || (option == "--no-debug"))
	  {
	    clog << "switching off debug mode." << endl;
	  }
	else
	  {
	    clog << "invalid option '" << option << "'" << endl;
	  }
      }

    if (command_name == "help")
      {
	clog << "print help" << endl;
      }
    else if (command_name == "exec")
      {
	if (number_of_arguments < 1)
	  {
	    throw datatools::utils::missing_argument ("Missing action name!");
	  }
	string action_name = command_.get_argument (0);
	clog << "execute action '" << action_name << "'" << endl;
      }
    else if (command_name == "run0")
      {
	if (number_of_arguments != 0)
	  {
	    throw datatools::utils::invalid_number_of_arguments ();
	  }
	clog << "execute 'run0'" << endl;	
      }
    else if (command_name == "run1")
      {
	if (number_of_arguments != 1)
	  {
	    throw datatools::utils::invalid_number_of_arguments ();
	  }
	clog << "execute 'run1' with argument '" 
		  << command_.get_argument (0) << "'" << endl;	
      }
    else if (command_name == "run2")
      {
	if (number_of_arguments != 2)
	  {
	    throw datatools::utils::invalid_number_of_arguments ();
	  }
	clog << "execute 'run2' with argument '" 
		  << command_.get_argument (0) << "' and '" 
		  << command_.get_argument (1) << "'" 
		  << endl;	
      }
    else
      {
	throw datatools::utils::command_not_found (command_name);
      }
  }
  
};

int 
main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Hello, this is a sample program for class 'command'!" << endl; 
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
	du::command my_command;
	
	my_command.set_name ("exec");
	my_command.add_argument ("action_name");
	my_command.add_argument ("action_arg[0]");
	my_command.add_argument ("action_arg[1]");
	my_command.add_argument ("action_arg[2]");
	my_command.set_error_code (du::command::SUCCESS);
	my_command.set_error_message ("It's ok!");
	
	my_command.dump (clog);

	test my_test (debug);
	if (my_test.invoke (my_command) != du::command::SUCCESS)
	  {
	    clog << "error message: " << my_command.get_error_message ()
		      << endl;
	  }
      }

      // another test bloc:
      {
	test the_test (debug);
	clog << "starting test shell..." << endl;
	string command_str;
	bool stop = false;
	do 
	  {
	    command_str = "";
	    clog << "test> ";
	    if (! getline (cin, command_str))
	      {
		stop = true;
		break;
	      }
	    if (command_str.empty ()) continue;
	    du::command the_command;
	    the_command.init (command_str);
	    if (the_test.invoke (the_command) != du::command::SUCCESS)
	      {
		clog << "test: error: " << the_command.get_error_message ()
			  << endl;
	      }
	  } while (! stop);
	if (command_str.empty ()) clog << endl;
	clog << "test shell stopped." << endl;
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

// end of test_command.cxx
