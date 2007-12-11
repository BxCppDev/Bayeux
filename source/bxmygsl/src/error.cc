// mygsl::error.cc

#include <mygsl/error.h>

namespace mygsl {
  
  void error::to_string( const int gsl_errno_ , 
			 std::string & err_string_ )
  {
    err_string_ = gsl_strerror(gsl_errno_);
  }
  
  std::string error::to_string( const int gsl_errno_ )
  {
    return std::string(gsl_strerror(gsl_errno_));
  }

  /*
  void error::throw_error( const std::string & reason_ , int gsl_errno_ )
  {
    GSL_ERROR(reason_.c_str(),gsl_errno_);
  }
 
  void throw_error( const std::string & reason_ , int gsl_errno_ , double value_ )
  {
    GSL_ERROR_VAL(reason_.c_str(),gsl_errno_,value_);
  }
  */

  void error::off()
  {
    __previous_handler = gsl_set_error_handler_off();
  }

  void error::on()
  {
    if ( __previous_handler != 0 ) {
      gsl_set_error_handler(__previous_handler);
      __previous_handler = 0;
    }
  }

  void error::set_handler( gsl_error_handler_t * handler_ )
  {
    if ( error::__builtin_handler == 0 ) {
      error::__builtin_handler = gsl_set_error_handler(handler_); 
    }
  }

  void error::default_handler( const char * reason_ ,
			       const char * file_ ,
			       int line_ ,
			       int gsl_errno_ ) 
  {
    std::ostringstream message;
    message << file_ << ':' << line_  
	    << ": " << reason_;
    if ( gsl_errno_ != 0 ) {
      message << ": " << error::to_string(gsl_errno_);
    }
    throw std::runtime_error(message.str());
  }

  gsl_error_handler_t * error::__builtin_handler=0;
  gsl_error_handler_t * error::__previous_handler=0;

  error::error()
  {
    error::set_handler(&error::default_handler); 
  }

  error * error::__g_error = new error();

}

// end of mygsl::error.cc

