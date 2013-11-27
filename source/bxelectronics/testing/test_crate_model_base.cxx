// Standard library
#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/crate_model_base.h>
#include <electronics/board_model_base.h>
#include <electronics/component_types.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    // Model of the control FPGA:
    electronics::component_handle_type ctrl_fpga_handle;
    ctrl_fpga_handle.reset(new electronics::component_model_base);
    electronics::component_model_base & ctrl_fpga = ctrl_fpga_handle.grab();
    ctrl_fpga.set_name("CTRL_FPGA");
    ctrl_fpga.set_display_name("Control FPGA");
    ctrl_fpga.set_terse_description("Control FPGA");
    ctrl_fpga.set_type(electronics::TYPE_CHIP_PLD);
    ctrl_fpga.grab_auxiliaries().store_string("manufacturer", "ALTERA");
    ctrl_fpga.grab_auxiliaries().store_string("serial_number", "?");
    ctrl_fpga.grab_auxiliaries().store_string("model.label", "Cyclone");
    ctrl_fpga.grab_auxiliaries().store_string("model.version", "II");
    ctrl_fpga.grab_auxiliaries().store_string("firmware.label", "CTRL");
    ctrl_fpga.grab_auxiliaries().store_string("firmware.version", "1.0");
    ctrl_fpga.tree_dump(std::cout, "CTRL_FPGA: ");
    ctrl_fpga.initialize_simple();

    // Model of Control Board:
    electronics::component_handle_type ctrl_board_handle;
    ctrl_board_handle.reset(new electronics::board_model_base);
    electronics::board_model_base & ctrl_board
      = dynamic_cast<electronics::board_model_base &>(ctrl_board_handle.grab());
    ctrl_board.set_name("CB");
    ctrl_board.set_display_name("Control board");
    ctrl_board.set_terse_description("A dummy control board");
    ctrl_board.grab_auxiliaries().store_string("model.label", "Dummy");
    ctrl_board.grab_auxiliaries().store_string("model.version", "1");
    ctrl_board.grab_auxiliaries().store_string("manufacturer", "ACME");
    ctrl_board.grab_auxiliaries().store_string("serial_number", "00102030");
    ctrl_board.grab_auxiliaries().store_string("inventory_number", "CNRS-999-777-3232");
    ctrl_board.set_slot_width(1);
    ctrl_board.set_format("VME-6U");
    ctrl_board.add_embedded("CTRL_FPGA", ctrl_fpga_handle);
    ctrl_board.initialize_simple();

    // Model of FEB:
    electronics::component_handle_type feb_board_handle;
    feb_board_handle.reset(new electronics::board_model_base);
    electronics::board_model_base & feb_board
      = dynamic_cast<electronics::board_model_base &>(feb_board_handle.grab());
    feb_board.set_name("FEB");
    feb_board.set_display_name("Front end board");
    feb_board.set_terse_description("A dummy frontend board");
    feb_board.grab_auxiliaries().store_string("model.label", "Dummy");
    feb_board.grab_auxiliaries().store_string("model.version", "1");
    feb_board.grab_auxiliaries().store_string("manufacturer", "ACME");
    feb_board.grab_auxiliaries().store_string("serial_number", "00102030");
    feb_board.grab_auxiliaries().store_string("inventory_number", "CNRS-999-777-3232");
    feb_board.set_slot_width(1);
    feb_board.set_format("VME-6U");
    feb_board.set_max_number_of_mezzanine_boards(0);
    feb_board.add_embedded("CTRL_FPGA", ctrl_fpga_handle);
    feb_board.initialize_simple();

    // Model of crate:
    electronics::component_handle_type crate_handle;
    crate_handle.reset(new electronics::crate_model_base);
    electronics::crate_model_base & crate
      = dynamic_cast<electronics::crate_model_base &>(crate_handle.grab());
    crate.set_name("Crate");
    crate.set_display_name("Front end electronics crate");
    crate.set_terse_description("A dummy crate");
    crate.grab_auxiliaries().store_string("model.label", "Dummy");
    crate.grab_auxiliaries().store_string("model.version", "3");
    crate.grab_auxiliaries().store_string("manufacturer", "ACME");
    crate.grab_auxiliaries().store_string("serial_number", "2157615237");
    crate.grab_auxiliaries().store_string("inventory_number", "CNRS-777-534-3232");
    crate.set_format("VME-6U");
    crate.set_max_number_of_modules(8);
    for (int iboard = 0; iboard < 3; iboard++) {
      crate.add_module(iboard, feb_board_handle);
    }
    crate.add_module(5, ctrl_board_handle);
    crate.initialize_simple();
    crate.tree_dump(std::cout, "Virtual crate: ");

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
