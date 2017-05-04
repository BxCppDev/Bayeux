// mygsl::error.cc

// Ourselves:
#include <mygsl/error.h>

// Standard library:
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace mygsl {

  using namespace std;

  bool  error::_active_ = true;
  error error::_singleton_;

  void error::to_string (const int gsl_errno_,
                         std::string & err_string_)
  {
    err_string_ = gsl_strerror (gsl_errno_);
    return;
  }

  std::string error::to_string (const int gsl_errno_)
  {
    return std::string (gsl_strerror (gsl_errno_));
  }

  void error::set_gsl_default ()
  {
    gsl_set_error_handler (NULL);
    return;
  }

  void error::set_default ()
  {
    error::set_handler (error::default_handler);
    return;
  }

  void error::off ()
  {
    error::_active_ = false;
    return;
  }

  void error::on ()
  {
    error::_active_ = true;
    return;
  }

  void error::set_handler (gsl_error_handler_t & handler_)
  {
    gsl_set_error_handler_off ();
    gsl_set_error_handler (&handler_);
    return;
  }

  void error::default_handler (const char * reason_,
                               const char * file_,
                               int line_,
                               int gsl_errno_)
  {
    if (error::_active_)
      {
        std::ostringstream message;
        message << "mygsl::error::default_handler: "
                << file_ << ':' << line_
                << ": " << reason_;
        if (gsl_errno_ != 0)
          {
            message << ": " << error::to_string (gsl_errno_);
          }
        DT_THROW_IF(true, std::logic_error, message.str ());
      }
    return;
  }

  error::error ()
  {
    error::set_default ();
    return;
  }

  error::~error ()
  {
    return;
  }

}
