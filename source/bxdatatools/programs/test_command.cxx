// -*- mode: c++; -*- 
// test_command.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <datatools/utils/command.h>
#include <datatools/utils/i_invokable.h>

struct test : public datatools::utils::i_invokable
{
  bool debug;
  
public:
  // ctor:
  test (bool debug_ = false)
  {
    debug = debug_;
    //std::cerr << "DEVEL: test::ctor: debug=" << debug << std::endl;
  }
  
  void 
  do_invoke (datatools::utils::command & command_)
  {
    //if (debug) std::clog << "test::do_invoke: entering..." << std::endl;
    command_.reset_output ();
      
    std::string command_name        = command_.get_name ();
    size_t      number_of_arguments = command_.get_number_of_arguments ();
    size_t      number_of_options   = command_.get_number_of_options ();
      
    if (number_of_options > 0)
      {
	std::string option  = command_.get_option (0);
	if ((option == "-d") || (option == "--debug"))
	  {
	    std::clog << "switching on debug mode." << std::endl;
	  }
	else if ((option == "-D") || (option == "--no-debug"))
	  {
	    std::clog << "switching off debug mode." << std::endl;
	  }
	else
	  {
	    std::clog << "invalid option '" << option << "'" << std::endl;
	  }
      }

    if (command_name == "help")
      {
	std::clog << "print help" << std::endl;
      }
    else if (command_name == "exec")
      {
	if (number_of_arguments < 1)
	  {
	    throw datatools::utils::missing_argument ("Missing action name!");
	  }
	std::string action_name = command_.get_argument (0);
	std::clog << "execute action '" << action_name << "'" << std::endl;
      }
    else if (command_name == "run0")
      {
	if (number_of_arguments != 0)
	  {
	    throw datatools::utils::invalid_number_of_arguments ();
	  }
	std::clog << "execute 'run0'" << std::endl;	
      }
    else if (command_name == "run1")
      {
	if (number_of_arguments != 1)
	  {
	    throw datatools::utils::invalid_number_of_arguments ();
	  }
	std::clog << "execute 'run1' with argument '" 
		  << command_.get_argument (0) << "'" << std::endl;	
      }
    else if (command_name == "run2")
      {
	if (number_of_arguments != 2)
	  {
	    throw datatools::utils::invalid_number_of_arguments ();
	  }
	std::clog << "execute 'run2' with argument '" 
		  << command_.get_argument (0) << "' and '" 
		  << command_.get_argument (1) << "'" 
		  << std::endl;	
      }
    else
      {
	throw datatools::utils::command_not_found (command_name);
      }
    //if (debug) std::clog << "test::do_invoke: exiting." << std::endl;
  }
  
};

int 
main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      std::cerr << "Hello, this is a sample program for class 'command'!" << std::endl; 
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
	du::command my_command;
	
	my_command.set_name ("exec");
	my_command.add_argument ("action_name");
	my_command.add_argument ("action_arg[0]");
	my_command.add_argument ("action_arg[1]");
	my_command.add_argument ("action_arg[2]");
	my_command.set_error_code (du::command::SUCCESS);
	my_command.set_error_message ("");
	
	my_command.dump (std::clog);

	test my_test (debug);
	if (my_test.invoke (my_command) != du::command::SUCCESS)
	  {
	    std::cerr << "error message: " << my_command.get_error_message ()
		      << std::endl;
	  }
      }

      // another test bloc:
      {
	test the_test (debug);
	std::clog << "starting test shell..." << std::endl;
	std::string command_str;
	bool stop = false;
	do 
	  {
	    command_str = "";
	    std::clog << "test> ";
	    if (! std::getline (std::cin, command_str))
	      {
		stop = true;
		break;
	      }
	    if (command_str.empty ()) continue;
	    du::command the_command;
	    the_command.init (command_str);
	    if (the_test.invoke (the_command) != du::command::SUCCESS)
	      {
		std::cerr << "test: error: " << the_command.get_error_message ()
			  << std::endl;
	      }
	  } while (! stop);
	if (command_str.empty ()) std::clog << std::endl;
	std::clog << "test shell stopped." << std::endl;
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

// end of test_command.cxx
