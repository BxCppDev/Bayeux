// command.h
#ifndef DATATOOLS_COMMAND_H_
#define DATATOOLS_COMMAND_H_
// Standard Library
#include <cstdlib> 
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

// Third Party
// - A

// This Project

namespace datatools {

class command;

class i_run_command {
 public:
  virtual void run_command(const command& cmd) = 0;
  virtual void print_help(std::ostream& out) const = 0;
};

//----------------------------------------------------------------------
// Command exception classes

/**! \class command_error
 * \brief The base class for command exception classes
 */
class command_error : public std::logic_error {
 public:
  command_error(const std::string& message = "");
};

/**! \class command_not_found
 * \brief A command exception 
 */
class command_not_found : public command_error {
 public:
  command_not_found(const std::string& message = "");
};


/**! \class missing_argument
 * \brief A command exception 
 */
class missing_argument : public command_error {
 public:
  missing_argument(const std::string& message = "");
};



/**! \class invalid_argument
 * \brief A command exception 
 */
class invalid_argument : public command_error {
 public:
  invalid_argument(const std::string& message = "");
};



/**! \class invalid_number_of_arguments
 * \brief A command exception 
 */
class invalid_number_of_arguments : public command_error {
 public:
  invalid_number_of_arguments(const std::string& message = "");
};


/**! \class command_failed
 * \brief A command exception 
 */
class command_failed : public command_error {
 public:
  command_failed(const std::string & message = "");
};


/**! \class command_not_implemented
 * \brief A command exception 
 */
class command_not_implemented : public command_error {
 public:
  command_not_implemented(const std::string& message = "");
};


//----------------------------------------------------------------------
// command class

/**! \brief Command class (not used yet)
 */
class command {
 public:
  enum {
    PASS    = -1,
    SUCCESS = EXIT_SUCCESS,
    ERROR   = EXIT_FAILURE,
    FAILURE = EXIT_FAILURE
  };

  static const char        OPTION_PREFIX;
  static const std::string LONG_OPTION_PREFIX;

 public:
  static bool token_is_option(const std::string& token);
  static bool token_is_short_option(const std::string& token);
  static bool token_is_long_option (const std::string& token);

  static bool code_is_pass(int /*code*/);
  static bool code_is_error(int /*code*/);
  static bool code_is_failure(int /*code*/);
  static bool code_is_success(int /*code*/);

 public:
  command();
  explicit command(const std::string& command_line);
  command(const std::string& command_line, bool with_options_);

  ~command();

  bool is_with_options() const;

  bool has_option(const std::string& /*option*/) const;

  void set_returns(const std::string& ret = "");

  int get_error_code() const;
  std::string get_returns() const;
  std::string get_error_message() const;
  std::string get_name() const;
  std::string get_argument(size_t /*arg*/ = 0) const;
  std::string get_option(size_t /*arg*/ = 0) const;

  bool is_pass() const;
  bool is_error() const;
  bool is_failure() const;
  bool is_success() const;

  void set_pass();
  void set_error();
  void set_success();

  void set_error_code(int /*code*/ = SUCCESS);
  void set_error_message(const std::string& /*msg*/);
  void set_name(const std::string& name);

  size_t get_number_of_arguments() const;
  void add_argument(const std::string& argument);
  std::string pop_argument();

  size_t get_number_of_options() const;
  void add_option(const std::string& option);
  std::string pop_option();

  void force_with_options();
  void reset_output ();

  void reset();

  void init(const std::string& command_line);

  void init(const std::string& command_line, bool with_options);

  void shift();

  void shift(int);

  void shift_one();

  void dump(std::ostream& out = std::clog) const;

  void dump_def () const {
    this->dump(std::cout);
  }

  void dump_stderr() const {
    this->dump(std::cerr);
  }

  void dump_stdout() const {
    this->dump(std::cout);
  }

 private:
  bool                   with_options_;
  std::string            name_;
  std::list<std::string> arguments_;
  std::list<std::string> options_;
  int                    error_code_;
  std::string            error_message_;
  std::string            returns_;
};

} // end of namespace datatools

#endif // DATATOOLS_COMMAND_H_

