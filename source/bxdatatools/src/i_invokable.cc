/* i_invokable.cc */
// Ourselves
#include <datatools/utils/i_invokable.h>

// Standard Library
#include <iostream>
#include <stdexcept>

// Third Party
// - A

// This Project
#include <datatools/utils/command.h>

namespace datatools {
namespace utils {

// default implementation:
void i_invokable::do_invoke(datatools::utils::command& command) {
  throw command_not_implemented("'i_invokable::do_invoke' method should be implemented from a concrete daughter class !"); 
}


int i_invokable::invoke(datatools::utils::command& command) {
  try {
    command.reset_output ();
    this->do_invoke(command); 
  }
  catch (std::runtime_error& x) {
    if (command.is_success()) {
      command.set_error();
      if (command.get_error_message().empty()) {
        command.set_error_message(x.what());
      }
    }
  }
  catch (...) {
    if (command.is_success()) {
      command.set_error();
      if (command.get_error_message().empty()) {
        command.set_error_message("unexpected error!");
      }
    }
  }
  return command.get_error_code();
}


int i_invokable::invoke(i_invokable& invokable, 
                        datatools::utils::command& command) {
  return invokable.invoke(command);
}

} // end of namespace utils 
} // end of namespace datatools 

/* end of i_invokable.cc */
