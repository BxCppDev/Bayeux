// mygsl::error.h

#ifndef __mygsl__error_h 
#define __mygsl__error_h 1

#include <gsl/gsl_errno.h>

#include <string>
#include <sstream>
#include <stdexcept>

namespace mygsl {


  class error
    {
    private:

      static gsl_error_handler_t * __builtin_handler;
      static gsl_error_handler_t * __previous_handler;
      static error * __g_error;

      error();

    public:

      static void to_string( const int gsl_errno_ , 
			     std::string & err_string_ );

      static std::string to_string( const int gsl_errno_ );

      /*	
      static void throw_error( const std::string & reason_ , 
			       int gsl_errno_ );

      static void throw_error( const std::string & reason_ , 
			       int gsl_errno_ , double value_ );
      */

    public:

      static void set_handler( gsl_error_handler_t * );

      static void off();

      static void on();

      static void default_handler( const char * reason_ ,
				   const char * file_ ,
				   int line_ ,
				   int gsl_errno_ );

    };
 
}

#endif //  __mygsl__error_h

// end of mygsl::error.h
