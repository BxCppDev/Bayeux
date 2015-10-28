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

  command::returned_info::returned_info(const std::string & output_)
  {
    set_error_code(CEC_SUCCESS);
    set_output(output_);
    return;
  }

  command::returned_info::returned_info(error_code_type code_,
                                        const std::string & message_)
  {
    set_error_code(code_);
    set_error_message(message_);
    return;
  }

  void command::returned_info::set_continue()
  {
    _error_code_ = CEC_CONTINUE;
    return;
  }

  void command::returned_info::set_stop()
  {
    _error_code_ = CEC_STOP;
    return;
  }

  void command::returned_info::set_success(const std::string & output_)
  {
    _error_code_ = CEC_SUCCESS;
    _error_message_.clear();
    set_output(output_);
    return;
  }

  void command::returned_info::set_failure(error_code_type code_,
                                           const std::string & message_)
  {
    set_error_code(code_);
    set_error_message(message_);
    _output_.clear();
    return;
  }

  bool command::returned_info::is_stop() const
  {
    return _error_code_ == CEC_STOP;
  }

  bool command::returned_info::is_success() const
  {
    return _error_code_ == CEC_SUCCESS;
  }

  bool command::returned_info::is_failure() const
  {
    return _error_code_ > CEC_SUCCESS;
  }

  bool command::returned_info::error_is(error_code_type code_) const
  {
    return _error_code_ == code_;
  }

  void command::returned_info::reset()
  {
    set_success();
    _output_.clear();
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

  bool command::returned_info::has_error_message() const
  {
    return ! _error_message_.empty();
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

  bool command::returned_info::has_output() const
  {
    return ! _output_.empty();
  }

  void command::returned_info::set_output(const std::string & output_)
  {
    _output_ = output_;
    return;
  }

  const std::string& command::returned_info::get_output() const
  {
    return _output_;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const command::returned_info & ri_)
  {
    out_ << '[' << "code=" << ri_.get_error_code() << "; ";
    if (ri_.is_success()) {
      out_ << "output=" << ri_.get_output();
    } else {
      out_ << "message=" << ri_.get_error_message();
    }
    out_ << ']';
    return out_;
  }

} // end of namespace datatools
