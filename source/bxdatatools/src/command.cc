// command.cc 
  
#include <datatools/utils/command.h>

#include <sstream>
#include <algorithm>

namespace datatools {

  namespace utils {

    const char        command::OPTION_PREFIX = '-';
    const std::string command::LONG_OPTION_PREFIX = "--";

    bool 
    command::token_is_option (const std::string & str_)
    {
      if (str_.empty ()) return false;
      return  (str_[0] == OPTION_PREFIX);
    }

    bool 
    command::token_is_short_option (const std::string & str_)
    {
      if (! token_is_option (str_)) return false;
      return ! token_is_long_option (str_);
    }

    bool 
    command::token_is_long_option (const std::string & str_)
    {
      if (! token_is_option (str_)) return false;
      if (str_.substr (0, 2) == LONG_OPTION_PREFIX) return true;
      return false;
    }

    bool command::code_is_pass (int code_)
    {
      return code_ == PASS;
    }
    
    bool command::code_is_error (int code_)
    {
      return (! code_is_pass (code_)) && (! code_is_success (code_));
    }
    
    bool command::code_is_failure (int code_)
    {
      return code_is_error (code_);
    }
    
    bool command::code_is_success (int code_)
    {
      return code_ == SUCCESS;
    }

    bool 
    command::is_with_options () const
    {
      return _with_options_;
    }

    bool 
    command::has_option (const std::string & opt_) const
    {
      return (std::find (_options_.begin (),
                         _options_.end (), opt_) != _options_.end ());
    }

    void  
    command::force_with_options ()
    {
      _with_options_ = true;
      std::list<std::string> arg_tmp = _arguments_;
      _arguments_.clear ();
      for (std::list<std::string>::const_iterator i 
             = arg_tmp.begin ();
           i != arg_tmp.end ();
           i++)
        {
          if (token_is_option (*i))
            {
              add_option (*i);
            }
          else
            {
              add_argument (*i);
            }
        }
      return;
    }

    std::string 
    command::get_name () const
    {
      return _name_;
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
      return _error_code_ == SUCCESS;
    }

    bool 
    command::is_pass () const
    {
      return _error_code_ == PASS;
    }

    std::string 
    command::get_returns () const
    {
      return _returns_;
    }

    void 
    command::set_returns (const std::string & ret_)
    {
      _returns_ = ret_;
      return;
    }
    
    void
    command::set_pass ()
    {
      set_error_code (PASS);
      return;
    }
    
    void
    command::set_error ()
    {
      set_error_code (ERROR);
      return;
    }
    
    void
    command::set_success ()
    {
      set_error_code (SUCCESS);
      return;
    }

    void
    command::set_error_code (int code_)
    {
      _error_code_ = code_;
      return;
    }
  
    void 
    command::set_error_message (const std::string & msg_)
    {
      _error_message_ = msg_;
    }

    /*
      void 
      command::set_error_message (const char * msg_)
      {
      set_error_message (std::string (msg_));
      }
    */

    int 
    command::get_error_code () const
    {
      return _error_code_;
    }

    std::string 
    command::get_error_message () const
    {
      return _error_message_;
    }

    void 
    command::set_name (const std::string & name_)
    {
      _name_ = name_;
      return;
    }

    size_t
    command::get_number_of_arguments () const
    {
      return _arguments_.size ();
    }

    void 
    command::add_argument (const std::string & arg_)
    {
      _arguments_.push_back (arg_);
      return;
    }

    std::string 
    command::get_argument (int i_) const
    {
      if ( (i_ < 0) || (i_ >= (int) _arguments_.size ())) 
        {
          return std::string ("");
        }
      int count = 0;
      for (std::list<std::string>::const_iterator i 
             = _arguments_.begin ();
           i != _arguments_.end ();
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
      if (_arguments_.size () > 0) 
        {
          std::string arg = _arguments_.front ();
          _arguments_.pop_front ();  
          return arg;
        }
      return std::string ("");
    }
  
    /*
      std::string 
      command::get_argument (int i_) const
      {
      if (i_ < 0 || i_ >= _arguments_.size ())
      {
      return std::string ();
      }
      else 
      {
        
      }
      return _arguments_;
      }
    */

    void
    command::reset_output ()
    {
      _returns_ = "";
      _error_code_ = SUCCESS;
      _error_message_ = "";
      return;
    }

    void
    command::reset ()
    {
      _name_ = "";
      _arguments_.clear ();
      _options_.clear ();
      reset_output ();
    }

    command::command ()
    {
      _with_options_ = false;
      _name_ = "";
      _returns_ = "";
      _error_code_ = SUCCESS;
      _error_message_ = "";
      return;
    }

    command::command (const std::string & command_line_)
    {
      _with_options_ = false;
      _name_ = "";
      _returns_ = "";
      _error_code_ = SUCCESS;
      _error_message_ = "";
      if (! command_line_.empty ())
        {
          init (command_line_, _with_options_);
        }
      return;
    }

    command::command (const std::string & command_line_, 
                      bool with_options_)
    {
      _with_options_ = with_options_;
      _name_ = "";
      _returns_ = "";
      _error_code_ = SUCCESS;
      _error_message_ = "";
      if (! command_line_.empty ())
        {
          init (command_line_, _with_options_);
        }
      return;
    }

    command::~command ()
    {
      reset ();
      return;
    }

    std::string 
    command::get_option (int i_) const
    {
      if ( (i_ < 0) || (i_ >= (int) _options_.size ())) 
        {
          return std::string ("");
        }
      int count = 0;
      for (std::list<std::string>::const_iterator i 
             = _options_.begin ();
           i != _options_.end ();
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
      return _options_.size ();
    }
    
    void 
    command::add_option (const std::string & opt_)
    {
      if (is_with_options ())
        {
          _options_.push_back (opt_);
        }
      else
        {
          throw std::runtime_error ("command::add_option: Options are not allowed!");
        }
    }
    
    std::string 
    command::pop_option ()
    {
      if (_options_.size () > 0) 
        {
          std::string arg = _options_.front ();
          _options_.pop_front ();  
          return arg;
        }
      return std::string ("");
    }

    void 
    command::init (const std::string & command_line_)
    {
      this->init (command_line_, false);
      return;
    }
  
    void 
    command::init (const std::string & command_line_, bool with_options_)
    {
      reset ();
      _with_options_ = with_options_;
      std::istringstream iss (command_line_);
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
              if (token_is_option (a_token) && is_with_options ()) 
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
      return;
    }

    void 
    command::shift (int pos_)
    {
      if (pos_<1) return;
      for (int i=0; i<pos_; i++)
        {
          _name_ = _arguments_.front ();
          _arguments_.pop_front ();  
        }
      return;
    }
  
    /* 
       void 
       command::shift_option (int pos_)
       {
       if (pos_<1) return;
       for (int i=0; i<pos_; i++)
       {
       _options_.pop_front ();  
       }
       }
    */

    void 
    command::shift ()
    {
      shift_one ();
      return;
    }
  
    /*
      void 
      command::shift_option ()
      {
      shift_option (0);
      }
    */

    void 
    command::shift_one ()
    {
      shift (1);
      return;
    }

    void 
    command::dump (std::ostream & out_) const
    {
      out_ << "datatools::utils::command: " << std::endl;
      out_ << "|--" << "Name : " << _name_ << std::endl;
      int i = 0;
      out_ << "|--" << "With options : " 
           << (is_with_options ()? "Yes": "No") << std::endl;
      if (is_with_options ())
        {
          for (std::list<std::string>::const_iterator it = 
                 _options_.begin ();
               it != _options_.end ();
               it++)
            {
              out_ << "|--" << "Options #" << i 
                   << ":  '" << *it << "'" << std::endl;
              i++;
            }
          i = 0;
        }
      for (std::list<std::string>::const_iterator it = 
             _arguments_.begin ();
           it != _arguments_.end ();
           it++)
        {
          out_ << "|--" << "Argument #" << i 
               << ":  '" << *it << "'" << std::endl;
          i++;
        }
      out_  << "|--" << "Error code : " << _error_code_ << std::endl;
      out_  << "`--" << "Error message : '" << _error_message_ << "'" << std::endl;
      return;
    }

    /* 
       int
       command::command_invoke (i_run_command & source_, const command & command_)
       {
       try
       {
       source_.run_command (command_);
       }
       catch (...)
       {
       }
       return 0;
       }
    */

    /*****************************************************/

    command_error::command_error (const std::string & message_) :
      std::runtime_error ("command: error: " + message_)
    {
      return;
    }

    command_not_found::command_not_found (const std::string & message_) :
      command_error ("command not found: " + message_)
    {
      return;
    }

    missing_argument::missing_argument (const std::string & message_) :
      command_error ("missing argument: " + message_)
    {
      return;
    }

    invalid_argument::invalid_argument (const std::string & message_) :
      command_error ("invalid argument: " + message_)
    {
      return;
    }

    invalid_number_of_arguments::invalid_number_of_arguments (const std::string & message_) :
      command_error ("invalid number of arguments: " + message_)
    {
      return;
    }

    command_failed::command_failed (const std::string & message_) :
      command_error ("command failed: " + message_)
    {
      return;
    }

    command_not_implemented::command_not_implemented (const std::string & message_) :
      command_error ("command not implemented: " + message_)
    {
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools

// end of command.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
