// command.cc 
// Ourself  
#include <datatools/command.h>

// Standard Library
#include <algorithm>
#include <sstream>

// Third Party
// - A

// This Project

namespace datatools {

const char        command::OPTION_PREFIX = '-';
const std::string command::LONG_OPTION_PREFIX = "--";

//----------------------------------------------------------------------
// command class definitions

command::command() {
  with_options_ = false;
  name_ = "";
  returns_ = "";
  error_code_ = SUCCESS;
  error_message_ = "";
}


command::command(const std::string& command_line) {
  with_options_ = false;
  name_ = "";
  returns_ = "";
  error_code_ = SUCCESS;
  error_message_ = "";
  if (!command_line.empty()) {
    this->init(command_line, with_options_);
  }
}


command::command(const std::string& command_line, bool with_options) {
  with_options_ = with_options;
  name_ = "";
  returns_ = "";
  error_code_ = SUCCESS;
  error_message_ = "";
  if (!command_line.empty()) {
    this->init(command_line, with_options_);
  }
}


command::~command() {
  this->reset();
}


bool command::token_is_option(const std::string& token) {
      if (token.empty()) return false;
      return token[0] == OPTION_PREFIX;
}


bool command::token_is_short_option(const std::string& token) {
      if (!token_is_option(token)) return false;
      return !token_is_long_option(token);
}

bool command::token_is_long_option(const std::string& token) {
  if (!token_is_option(token)) return false;
  if (token.substr(0,2) == LONG_OPTION_PREFIX) return true;
  return false;
}


bool command::code_is_pass(int code) {
  return code == PASS;
}


bool command::code_is_error(int code) {
  return (!code_is_pass(code)) && (!code_is_success(code));
}


bool command::code_is_failure(int code) {
  return code_is_error(code);
}


bool command::code_is_success(int code) {
  return code == SUCCESS;
}


bool command::is_with_options() const {
  return with_options_;
}

bool command::has_option(const std::string& option) const {
  return (std::find(options_.begin(), options_.end(), option) 
          != options_.end());
}


void command::force_with_options() {
  with_options_ = true;
  std::list<std::string> arg_tmp = arguments_;
  arguments_.clear();
  for (std::list<std::string>::const_iterator i = arg_tmp.begin();
       i != arg_tmp.end();
       i++) {
    if (this->token_is_option(*i)) {
      this->add_option(*i);
    }
    else {
      this->add_argument(*i);
    }
  }
}


std::string command::get_name() const {
  return name_;
}


bool command::is_error() const {
  return (!this->is_pass()) && (!this->is_success());
}


bool command::is_failure() const {
  return this->is_error();
}


bool command::is_success() const {
  return error_code_ == SUCCESS;
}


bool command::is_pass() const {
  return error_code_ == PASS;
}


std::string command::get_returns() const {
  return returns_;
}


void command::set_returns(const std::string& ret_) {
  returns_ = ret_;
}


void command::set_pass() {
  this->set_error_code(PASS);
}


void command::set_error() {
  this->set_error_code(ERROR);
}


void command::set_success() {
  this->set_error_code(SUCCESS);
}


void command::set_error_code (int code) {
  error_code_ = code;
}


void command::set_error_message(const std::string& msg) {
  error_message_ = msg;
}


int command::get_error_code() const {
  return error_code_;
}


std::string command::get_error_message() const {
  return error_message_;
}


void command::set_name(const std::string& name) {
  name_ = name;
}


size_t command::get_number_of_arguments() const {
  return arguments_.size();
}


void command::add_argument(const std::string& arg) {
  arguments_.push_back(arg);
}


std::string command::get_argument(size_t i) const {
  if (i >= arguments_.size()) {
    return std::string("");
  }

  size_t count = 0;
  for (std::list<std::string>::const_iterator it = arguments_.begin();
       it != arguments_.end();
       ++it) {
    if (count == i) return *it;
    count++;
  }
  return std::string("");
}


std::string command::pop_argument() {
  if (arguments_.size () > 0) {
    std::string arg = arguments_.front();
    arguments_.pop_front();  
    return arg;
  }
  return std::string ("");
}


void command::reset_output() {
  returns_ = "";
  error_code_ = SUCCESS;
  error_message_ = "";
}


void command::reset() {
  name_ = "";
  arguments_.clear();
  options_.clear();
  this->reset_output();
}


std::string command::get_option(size_t i) const {
  if (i >= options_.size()) {
    return std::string ("");
  }
  size_t count = 0;
  for (std::list<std::string>::const_iterator it = options_.begin();
       it != options_.end();
       ++it) {
    if (count == i) return *it;
    count++;
  }
  return std::string ("");
}


size_t command::get_number_of_options() const {
  return options_.size();
}


void command::add_option(const std::string& option) {
  if (this->is_with_options()) {
    options_.push_back(option);
  } else {
    throw std::runtime_error("command::add_option: Options are not allowed!");
  }
}


std::string command::pop_option() {
  if (options_.size() > 0) {
    std::string arg = options_.front();
    options_.pop_front();  
    return arg;
  }
  return std::string("");
}


void command::init(const std::string& command_line) {
  this->init(command_line, false);
}


void command::init(const std::string& command_line, bool with_options) {
  this->reset();
  with_options_ = with_options;
  std::istringstream iss(command_line);
  std::string a_command;
  iss >> a_command;
  this->set_name(a_command);
  iss >> std::ws;
  while (iss) {
    std::string a_token;    
    iss >> a_token;
    if (iss) {
      if (this->token_is_option(a_token) && this->is_with_options()) {
        this->add_option(a_token);
      } else {
        this->add_argument(a_token);
      }
    }
    iss >> std::ws;
  }
}


void command::shift(int by) {
  if (by < 1) return;
  for (int i=0; i < by ; ++i) {
    name_ = arguments_.front();
    arguments_.pop_front();  
  }
}


void command::shift() {
  this->shift_one();
}


void command::shift_one() {
  this->shift(1);
}


void command::dump(std::ostream& out) const {
  out << "datatools::command: " << std::endl;
  out << "|--" << "Name : " << name_ << std::endl;
  int i = 0;
  out << "|--" << "With options : " 
      << (this->is_with_options() ? "Yes" : "No") << std::endl;
  if (this->is_with_options()) {
    for (std::list<std::string>::const_iterator it = options_.begin();
         it != options_.end();
         ++it) {
      out << "|--" << "Options #" << i 
          << ":  '" << *it << "'" << std::endl;
      i++;
    }
    i = 0;
  }

  for (std::list<std::string>::const_iterator it = arguments_.begin();
       it != arguments_.end();
       ++it) {
    out << "|--" << "Argument #" << i 
        << ":  '" << *it << "'" << std::endl;
    i++;
  }
  out  << "|--" << "Error code : " << error_code_ << std::endl;
  out  << "`--" << "Error message : '" << error_message_ << "'" << std::endl;
}


//----------------------------------------------------------------------
// Define command exceptions

command_error::command_error(const std::string& message) 
    : std::logic_error("command: error: " + message) {}

    
command_not_found::command_not_found(const std::string& message)
    : command_error("command not found: " + message) {}

      
missing_argument::missing_argument(const std::string& message)
    : command_error("missing argument: " + message) {}

    
invalid_argument::invalid_argument(const std::string& message)
    : command_error("invalid argument: " + message) {}


invalid_number_of_arguments::invalid_number_of_arguments(
    const std::string& message) 
    : command_error("invalid number of arguments: " + message) {}


command_failed::command_failed(const std::string& message)
    : command_error("command failed: " + message) {}


command_not_implemented::command_not_implemented(
    const std::string& message) 
    : command_error("command not implemented: " + message) {}

} // end of namespace datatools

