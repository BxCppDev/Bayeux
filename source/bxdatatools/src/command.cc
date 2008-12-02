// command.cc
  
#include <datatools/utils/command.h>

namespace datatools {

  namespace utils {

    const char        command::OPTION_PREFIX = '-';
    const std::string command::LONG_OPTION_PREFIX = "--";

    bool 
    command::is_option (std::string str_)
    {
      if (str_.empty ()) return false;
      return  (str_[0] == OPTION_PREFIX);
    }

    bool 
    command::is_short_option (std::string str_)
    {
      if (! is_option (str_)) return false;
      return ! is_long_option (str_);
    }

    bool 
    command::is_long_option (std::string str_)
    {
      if (! is_option (str_)) return false;
      if (str_.substr(0, 2) == LONG_OPTION_PREFIX) return true;
      return false;
    }

    bool command::is_pass (int code_)
    {
      return code_ == PASS;
    }
    
    bool command::is_error (int code_)
    {
      return (! is_pass (code_)) && (! is_success (code_));
    }
    
    bool command::is_failure (int code_)
    {
      return is_error (code_);
    }
    
    bool command::is_success (int code_)
    {
      return code_ == SUCCESS;
    }

    bool 
    command::is_with_options () const
    {
      return __with_options;
    }

    bool 
    command::has_option (std::string opt_) const
    {
      return (std::find (__options.begin (),
			 __options.end (), opt_) != __options.end ());
    }

    void  
    command::force_with_options ()
    {
      __with_options = true;
      std::list<std::string> arg_tmp = __arguments;
      __arguments.clear ();
      for (std::list<std::string>::const_iterator i 
	     = arg_tmp.begin ();
	   i != arg_tmp.end ();
	   i++)
	{
	  if (is_option (*i))
	    {
	      add_option (*i);
	    }
	  else
	    {
	      add_argument (*i);
	    }
	}
    }

    std::string 
    command::get_name () const
    {
      return __name;
    }

    bool
    command::is_error () const
    {
      return (! is_pass ()) && ! (is_success ());
    }
    
    bool
    command::is_failure () const
    {
      return is_error ();
    }
    
    bool 
    command::is_success () const
    {
      return __error_code == SUCCESS;
    }

    bool 
    command::is_pass () const
    {
      return __error_code == PASS;
    }

    std::string 
    command::get_returns () const
    {
      return __returns;
    }

    void 
    command::set_returns (std::string ret_)
    {
      __returns = ret_;
    }
    
    void
    command::set_pass ()
    {
      set_error_code (PASS);
    }
    
    void
    command::set_error ()
    {
      set_error_code (ERROR);
    }
    
    void
    command::set_success ()
    {
      set_error_code (SUCCESS);
    }

    void
    command::set_error_code (int code_)
    {
      __error_code = code_;
    }
  
    void 
    command::set_error_message (std::string msg_)
    {
      __error_message = msg_;
    }

    int 
    command::get_error_code () const
    {
      return __error_code;
    }

    std::string 
    command::get_error_message () const
    {
      return __error_message;
    }

    void 
    command::set_name (std::string name_)
    {
      __name = name_;
    }

    size_t
    command::get_number_of_arguments () const
    {
      return __arguments.size ();
    }

    void 
    command::add_argument (std::string arg_)
    {
      __arguments.push_back (arg_);
    }

    std::string 
    command::get_argument (int i_) const
    {
      if ((i_ < 0) || (i_ >= (int) __arguments.size ())) 
	{
	  return std::string ("");
	}
      int count = 0;
      for (std::list<std::string>::const_iterator i 
	     = __arguments.begin ();
	   i != __arguments.end ();
	   i++)
	{
	  if (count == i_) return *i;
	  count++;
	}
      return std::string ("");
    }

    std::string 
    command::pop_argument ()
    {
      if (__arguments.size () > 0) 
	{
	  std::string arg = __arguments.front ();
	  __arguments.pop_front ();  
	  return arg;
	}
      return std::string ("");
    }
  
    /*
      std::string 
      command::get_argument (int i_) const
      {
      if (i_ < 0 || i_ >= __arguments.size())
      {
      return std::string();
      }
      else 
      {
	
      }
      return __arguments;
      }
    */

    void
    command::reset_output ()
    {
      __returns = "";
      __error_code = SUCCESS;
      __error_message = "";
    }

    void
    command::reset ()
    {
      __name = "";
      __arguments.clear ();
      __options.clear ();
      reset_output ();
    }

    command::command (const std::string & command_line_, 
		      bool with_options_)
    {
      __with_options = with_options_;
      __name = "";
      __returns = "";
      __error_code = SUCCESS;
      __error_message = "";
      if (! command_line_.empty ())
	{
	  init (command_line_, __with_options);
	}
    }

    command::~command ()
    {
      reset ();
    }

    std::string 
    command::get_option (int i_) const
    {
      if ((i_ < 0) || (i_ >= (int) __options.size ())) 
	{
	  return std::string ("");
	}
      int count = 0;
      for (std::list<std::string>::const_iterator i 
	     = __options.begin ();
	   i != __options.end ();
	   i++)
	{
	  if (count == i_) return *i;
	  count++;
	}
      return std::string ("");
    }


    size_t
    command::get_number_of_options () const
    {
      return __options.size ();
    }
    
    void 
    command::add_option (std::string opt_)
    {
      __options.push_back (opt_);
    }
    
    std::string 
    command::pop_option ()
    {
      if (__options.size () > 0) 
	{
	  std::string arg = __options.front ();
	  __options.pop_front ();  
	  return arg;
	}
      return std::string ("");
    }
  
    void 
    command::init (const std::string & command_line_, bool with_options_)
    {
      reset();
      __with_options = with_options_;
      std::istringstream iss(command_line_);
      std::string a_command;
      iss >> a_command;
      set_name (a_command);
      iss >> std::ws;
      while (iss) 
	{
	  std::string a_token;    
	  iss >> a_token;
	  if (iss) 
	    {
	      if (is_option (a_token) && is_with_options ()) 
		{
		  add_option (a_token);
		}
	      else
		{
		  add_argument (a_token);
		}
	    }
	  iss >> std::ws;
	}
    }

    void 
    command::shift(int pos_)
    {
      if (pos_<1) return;
      for (int i=0; i<pos_; i++)
	{
	  __name = __arguments.front();
	  __arguments.pop_front();  
	}
    }
  
    void 
    command::shift_one ()
    {
      shift (1);
    }

    void 
    command::dump(std::ostream & out_)
    {
      out_ << "Command: " << std::endl;
      out_ << "|--" << "Name : " << __name << std::endl;
      int i = 0;
      out_ << "|--" << "With options : " 
	   << (is_with_options ()? "Yes": "No") << std::endl;
      if (is_with_options ())
	{
	  for (std::list<std::string>::const_iterator it = 
		 __options.begin();
	       it != __options.end();
	       it++)
	    {
	      out_ << "|--" << "Options #" << i 
		   << ":  '" << *it << "'" << std::endl;
	      i++;
	    }
	  i = 0;
	}
      for (std::list<std::string>::const_iterator it = 
	     __arguments.begin();
	   it != __arguments.end();
	   it++)
	{
	  out_ << "|--" << "Argument #" << i 
	       << ":  '" << *it << "'" << std::endl;
	  i++;
	}
      out_  << "|--" << "Error code : " << __error_code << std::endl;
      out_  << "`--" << "Error message : '" << __error_message << "'" << std::endl;
    }

    /* 
       int
       command::command_invoke(i_run_command & source_, const command & command_)
       {
       try
       {
       source_.run_command(command_);
       }
       catch(...)
       {
       }
       return 0;
       }
    */

    /*****************************************************/

    command_error::command_error (const std::string & message_) :
      std::runtime_error("command: error: " + message_)
    {
    }

    command_not_found::command_not_found (const std::string & message_) :
      command_error ("command not found: " + message_)
    {
    }

    missing_argument::missing_argument (const std::string & message_) :
      command_error ("missing argument: " + message_)
    {
    }

    invalid_argument::invalid_argument (const std::string & message_) :
      command_error ("invalid argument: " + message_)
    {
    }

    invalid_number_of_arguments::invalid_number_of_arguments (const std::string & message_) :
      command_error ("invalid number of arguments: " + message_)
    {
    }

    command_failed::command_failed (const std::string & message_) :
      command_error ("command failed: " + message_)
    {
    }

    command_not_implemented::command_not_implemented (const std::string & message_) :
      command_error ("command not implemented: " + message_)
    {
    }

  } // end of namespace utils 

} // end of namespace datatools

// end of command.cc
