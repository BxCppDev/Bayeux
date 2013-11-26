#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/rack_base.h>
#include <electronics/crate_base.h>
#include <electronics/board_base.h>
#include <electronics/component_types.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    // Control FPGA:
    electronics::component_handle_type ctrl_fpga_handle;
    ctrl_fpga_handle.reset(new electronics::component_base);
    electronics::component_base & ctrl_fpga = ctrl_fpga_handle.grab();
    ctrl_fpga.set_name("FEB_CTRL_FPGA");
    ctrl_fpga.set_display_name("FEB Control FPGA");
    ctrl_fpga.set_terse_description("FEB Control FPGA");
    ctrl_fpga.set_type(electronics::TYPE_CHIP_PLD);
    ctrl_fpga.grab_auxiliaries().store_string("manufacturer", "ALTERA");
    ctrl_fpga.grab_auxiliaries().store_string("serial_number", "?");
    ctrl_fpga.grab_auxiliaries().store_string("model.label", "Cyclone");
    ctrl_fpga.grab_auxiliaries().store_string("model.version", "II");
    ctrl_fpga.grab_auxiliaries().store_string("firmware.label", "CTRL");
    ctrl_fpga.grab_auxiliaries().store_string("firmware.version", "1.0");
    ctrl_fpga.tree_dump(std::cout, "CTRL_FPGA: ");
    ctrl_fpga.initialize_simple();

    // Boards:
    electronics::component_handle_type board_handle;

    board_handle.reset(new electronics::board_base);
    electronics::board_base & board
      = dynamic_cast<electronics::board_base &>(board_handle.grab());
    board.set_name("FEB");
    board.set_display_name("Front end board");
    board.set_terse_description("A dummy frontend board");
    board.grab_auxiliaries().store_string("model.label", "Dummy");
    board.grab_auxiliaries().store_string("model.version", "1");
    board.grab_auxiliaries().store_string("manufacturer", "ACME");
    board.grab_auxiliaries().store_string("serial_number", "00102030");
    board.grab_auxiliaries().store_string("inventory_number", "CNRS-999-777-3232");
    board.set_slot_width(1);
    board.set_format("VME-6U");
    board.set_max_number_of_mezzanine_boards(0);
    board.add_embedded("CTRL_FPGA", ctrl_fpga_handle);
    board.grab_auxiliaries().store_string("mapping.embedded.CTRL_FPGA", "[feb_ctrl_fpga.ec]");
    board.initialize_simple();


    // Crate:
    electronics::component_handle_type crate_handle;
    crate_handle.reset(new electronics::crate_base);
    electronics::crate_base & crate
      = dynamic_cast<electronics::crate_base &>(crate_handle.grab());
    crate.set_name("CaloCrate");
    crate.set_display_name("Calorimeter Crate");
    crate.set_terse_description("A calorimeter crate");
    crate.grab_auxiliaries().store_string("model.label", "Dummy");
    crate.grab_auxiliaries().store_string("model.version", "3");
    crate.grab_auxiliaries().store_string("manufacturer", "ACME");
    crate.grab_auxiliaries().store_string("serial_number", "2157615237");
    crate.grab_auxiliaries().store_string("inventory_number", "CNRS-777-534-3232");
    crate.set_format("VME-6U");
    crate.set_max_number_of_modules(8);
    for (int iboard = 0; iboard < 3; iboard++) {
      crate.add_module(iboard, board_handle);
      std::ostringstream eid_rule;
      eid_rule << "[feb.ec:board_id=" << iboard << "]";
      crate.grab_auxiliaries().store_string(electronics::make_indexed_name("mapping.embedded.module_", iboard),
                                            eid_rule.str());
    }
    crate.initialize_simple();

    // Rack:
    electronics::component_handle_type rack_handle;
    rack_handle.reset(new electronics::rack_base);
    electronics::rack_base & rack
      = dynamic_cast<electronics::rack_base &>(rack_handle.grab());
    rack.set_name("CaloRack");
    rack.set_display_name("Calorimeter Rack");
    rack.set_terse_description("The calorimeter rack");
    rack.set_format("VME-6U");
    rack.set_max_number_of_crates(3);
    rack.add_crate(1, crate_handle);
    rack.grab_auxiliaries().store_string("mapping.embedded.crate_1", "[crate.ec:crate_id=1]");
    rack.initialize_simple();
    rack.tree_dump(std::cout, "Virtual Calorimeter Rack: ");

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
