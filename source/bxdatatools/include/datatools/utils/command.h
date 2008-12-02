// command.h

#ifndef __datatools__utils__command_h
#define __datatools__utils__command_h 1

#include <algorithm>
#include <cstdlib> 
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <list>

namespace datatools {

  namespace utils {

    class command;

    class i_run_command 
      {
      public:
	virtual void
	  run_command (const command &) = 0;
	virtual void 
	  print_help (std::ostream & out_) const = 0;
      };

    class command_error : public std::runtime_error
      {
      public:
	command_error (const std::string & message_ = "");
      };

    class command_not_found : public command_error
      {
      public:
	command_not_found (const std::string & message_ = "");
      };

    class missing_argument : public command_error
      {
      public:
	missing_argument (const std::string & message_ = "");
      };

    class invalid_argument : public command_error
      {
      public:
	invalid_argument (const std::string & message_ = "");
      };

    class invalid_number_of_arguments : public command_error
      {
      public:
	invalid_number_of_arguments (const std::string & message_ = "");
      };

    class command_failed : public command_error
      {
      public:
	command_failed (const std::string & message_ = "");
      };

    class command_not_implemented : public command_error
      {
      public:
	command_not_implemented (const std::string & message_ = "");
      };


    class command
      {

      public:
	enum
	  {
	    PASS    = -1,
	    SUCCESS = EXIT_SUCCESS,
	    ERROR   = EXIT_FAILURE,
	    FAILURE = EXIT_FAILURE
	  };
	
	static const char        OPTION_PREFIX;
	static const std::string LONG_OPTION_PREFIX;

      public:
	bool                   __with_options;
	std::string            __name;
	std::list<std::string> __arguments;
	std::list<std::string> __options;
	int                    __error_code;
	std::string            __error_message;
	std::string            __returns;
  
      public:

	static bool is_option (std::string str_);

	static bool is_short_option (std::string str_);

	static bool is_long_option (std::string str_);

	static bool is_pass (int);

	static bool is_error (int);

	static bool is_failure (int);

	static bool is_success (int);

	bool 
	  is_with_options () const;

	bool 
	  has_option (std::string) const;

	int 
	  get_error_code () const;

	void 
	  set_returns (std::string ret_ = "");

	std::string 
	  get_returns () const;

	std::string 
	  get_error_message () const;

	std::string 
	  get_name () const;

	std::string 
	  get_argument (int = 0) const;

	std::string 
	  get_option (int = 0) const;

	bool 
	  is_pass () const;

	bool
	  is_error () const;

	bool
	  is_failure () const;

	bool 
	  is_success () const;

	void
	  set_pass ();

	void
	  set_error ();

	void
	  set_success ();

	void
	  set_error_code (int = SUCCESS);

	void 
	  set_error_message (std::string);

	void 
	  set_name (std::string);

	size_t
	  get_number_of_arguments () const;

	void 
	  add_argument (std::string);

	std::string 
	  pop_argument ();

	size_t
	  get_number_of_options () const;

	void 
	  add_option (std::string);

	std::string 
	  pop_option ();

	void  
	  force_with_options ();

	command (const std::string & command_line_ = "", 
		 bool with_options_ = false);

	~command ();

	void
	  reset_output ();
  
	void
	  reset ();
  
	void 
	  init (const std::string & command_line_, bool with_options_ = false);

	void 
	  shift (int = 1);

	void 
	  shift_one ();

	void 
	  dump (std::ostream & out_ = std::cerr);

	/*
	  static int
	  command_invoke (i_run_command & source_, const command & command_);
	*/
      };

  } // end of namespace utils 

} // end of namespace datatools

#endif // __datatools__utils__command_h

// end of command.h
