#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/register_base.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    electronics::register_base reg0;
    reg0.set_name("CFGREG0");
    //reg0.set_logging_priority(datatools::logger::PRIO_DEBUG);
    reg0.set_display_name("Configuration register 0");
    reg0.set_terse_description("Configuration register");
    reg0.grab_auxiliaries().store_flag("test");
    reg0.set_size(electronics::register_base::SIZE_16);
    reg0.set_readable(true);
    reg0.set_writable(true);
    reg0.add_bitset("HMULT",  0,  3, "001");
    reg0.add_bitset("HTEST", 11,  1, "0");
    reg0.add_bitset("HPACK",  3,  8, "00001000");
    reg0.append_bitset("HDBG", 2, "00");
    reg0.initialize_simple();
    reg0.tree_dump(std::clog, "Register description: ");
    reg0.reset();
    reg0.tree_dump(std::clog, "Register description: ");

    std::cerr << "The end." << std::endl;

  }
  catch (std::exception& error) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,
                 error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,
                 "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
