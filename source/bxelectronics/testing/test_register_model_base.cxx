// Standard library
#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/register_model_base.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    electronics::register_model_base RegDesc;
    RegDesc.set_name("FOOREG");
    //RegDesc.set_logging_priority(datatools::logger::PRIO_DEBUG);
    RegDesc.set_display_name("Configuration register #0");
    RegDesc.set_terse_description("Configuration register");
    RegDesc.grab_auxiliaries().store_flag("test");
    RegDesc.set_size(electronics::register_model_base::SIZE_16);
    RegDesc.set_readable(true);
    RegDesc.set_writable(true);
    RegDesc.add_bitset("HMULT",  0,  3, "001");
    RegDesc.add_bitset("HTEST", 11,  1, "1");
    RegDesc.add_bitset("HPACK",  3,  8, "00001110");
    RegDesc.append_bitset("HDBG",    2, "10");
    RegDesc.initialize_simple();
    RegDesc.tree_dump(std::clog, "Register description: ");

    boost::dynamic_bitset<> reg0;
    RegDesc.make(reg0);
    std::cerr << "Register : '" << reg0 << "'" << std::endl;


    RegDesc.reset();
    RegDesc.tree_dump(std::clog, "Register description: ");

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
