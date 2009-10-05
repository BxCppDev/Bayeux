// mygsl::error.cc

#include <mygsl/error.h>

namespace mygsl {
  
  void error::to_string (const int gsl_errno_, 
			 std::string & err_string_)
  {
    err_string_ = gsl_strerror (gsl_errno_);
  }
  
  std::string error::to_string (const int gsl_errno_)
  {
    return std::string (gsl_strerror (gsl_errno_));
  }

  void error::set_gsl_default ()
  {
    gsl_set_error_handler (NULL);
  }

  void error::set_default ()
  {
    error::set_handler (error::default_handler); 
  }

  void error::off ()
  {
   error::__active = false;
  }

  void error::on ()
  {
    error::__active = true;
  }

  void error::set_handler (gsl_error_handler_t & handler_)
  {
    gsl_set_error_handler_off (); 
    gsl_set_error_handler (&handler_); 
  }

  void error::default_handler (const char * reason_,
			       const char * file_,
			       int line_,
			       int gsl_errno_) 
  {
    if (error::__active) 
      {
	std::ostringstream message;
	message << "mygsl::error::default_handler: " 
		<< file_ << ':' << line_  
		<< ": " << reason_;
	if (gsl_errno_ != 0) 
	  {
	    message << ": " << error::to_string (gsl_errno_);
	  }
	throw std::runtime_error (message.str ());
      }
  }

  bool  error::__active = true;
  error error::__singleton;

  error::error ()
  {
    error::set_default ();
  }

  error::~error ()
  {
  }

}

// end of mygsl::error.cc

