// command_utils.cc

// Ourselves:
#include <datatools/command_utils.h>

// Standard Library:
#include <sstream>
#include <stdexcept>

// This project:
#include <datatools/exception.h>

namespace datatools {

  command::returned_info::returned_info()
  {
    set_error_code(CEC_SUCCESS);
    return;
  }

  command::returned_info::returned_info(error_code_type code_,
                                        const std::string & message_)
  {
    set_error_code(code_);
    set_error_message(message_);
    return;
  }

  bool command::returned_info::is_success() const
  {
    return _error_code_ == CEC_SUCCESS;
  }

  bool command::returned_info::is_failure() const
  {
    return !is_success();
  }

  void command::returned_info::reset()
  {
    _error_code_ = CEC_SUCCESS;
    _error_message_.clear();
    return;
  }

  void command::returned_info::set_error_code(error_code_type code_)
  {
    _error_code_ = code_;
    return;
  }

  command::error_code_type command::returned_info::get_error_code() const
  {
    return _error_code_;
  }

  void command::returned_info::set_error_message(const std::string & message_)
  {
    _error_message_ = message_;
    return;
  }

  const std::string& command::returned_info::get_error_message() const
  {
    return _error_message_;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const command::returned_info & ri_)
  {
    out_ << '[' << "code=" << ri_.get_error_code() << "; message=" << ri_.get_error_message() << ']';
    return out_;
  }

} // end of namespace datatools
