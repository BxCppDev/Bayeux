// STandard library
#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/component_types.h>
#include <electronics/component_model_base.h>
#include <electronics/component_utils.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    // Device model:
    electronics::component_model_base device;
    device.set_name("MyDevice");
    device.set_display_name("My device");
    device.set_terse_description("A mock electronic device");
    device.set_type(electronics::TYPE_DEVICE);
    device.initialize_simple();
    device.tree_dump(std::cout, "Virtual electronic device: ");
    std::cout << std::endl;

    // Setup model:
    electronics::component_model_base setup;
    setup.set_name("MySetup");
    setup.set_display_name("My setup");
    setup.set_terse_description("A dummy electronic setup");
    setup.set_type(electronics::TYPE_SETUP);
    setup.grab_auxiliaries().store_string("model.label", "Dummy");
    setup.grab_auxiliaries().store_string("model.version", "1.0");
    setup.grab_auxiliaries().store_string("model.manufacturer", "ACME");
    // Add register:
    {
      electronics::register_desc regmask0;
      regmask0.set_size(16);
      regmask0.set_readable(true);
      regmask0.set_writable(true);
      regmask0.append_bitset("B0", 3, "000");
      regmask0.append_bitset("B1", 1, "0");
      regmask0.append_bitset("B2", 8, "10000000");
      regmask0.initialize();
      regmask0.tree_dump(std::cout, "regmask0: ");
      std::cout << std::endl;
      setup.add_register("CFGMASK0", regmask0);
     }
    // Add devices in the setup:
    setup.add_embedded_component("device0", device);
    setup.add_embedded_component("device1", device);
    setup.add_embedded_component("device2", device);
    setup.tree_dump(std::cout, "Virtual electronic setup: ");
    std::cout << std::endl;
    setup.remove_embedded_component("device2");
    setup.initialize_simple();
    setup.tree_dump(std::cout, "Virtual electronic setup: ");
    std::cout << std::endl;

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
