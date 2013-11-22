#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/crate_base.h>
#include <electronics/board_base.h>
#include <electronics/component_types.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    // Crate:
    electronics::component_handle_type crate_handle;
    crate_handle.reset(new electronics::crate_base);
    electronics::crate_base & crate
      = dynamic_cast<electronics::crate_base &>(crate_handle.grab());
    crate.set_name(electronics::make_indexed_name("Crate_", 0));
    crate.set_display_name(electronics::make_indexed_name("Crate #", 0));
    crate.set_terse_description("A dummy crate");
    crate.grab_auxiliaries().store_string("model.label", "Dummy");
    crate.grab_auxiliaries().store_string("model.version", "3");
    crate.grab_auxiliaries().store_string("manufacturer", "ACME");
    crate.grab_auxiliaries().store_string("serial_number", "2157615237");
    crate.grab_auxiliaries().store_string("inventory_number", "CNRS-777-534-3232");
    crate.set_format("VME-6U");
    crate.set_max_number_of_modules(8);

    for (int iboard = 0; iboard < 3; iboard++) {
      // Boards:
      electronics::component_handle_type board_handle;

      board_handle.reset(new electronics::board_base);
      electronics::board_base & board
        = dynamic_cast<electronics::board_base &>(board_handle.grab());
      board.set_name(electronics::make_indexed_name("FEB_", iboard));
      board.set_display_name(electronics::make_indexed_name("FEB #", iboard));
      board.set_terse_description("A dummy frontend board");
      board.grab_auxiliaries().store_string("model.label", "Dummy");
      board.grab_auxiliaries().store_string("model.version", "1");
      board.grab_auxiliaries().store_string("manufacturer", "ACME");
      board.grab_auxiliaries().store_string("serial_number", "00102030");
      board.grab_auxiliaries().store_string("inventory_number", "CNRS-999-777-3232");
      board.set_slot_width(1);
      board.set_format("VME-6U");
      board.set_max_number_of_mezzanine_boards(0);
      {
        // Control FPGA:
        electronics::component_handle_type ctrl_fpga_handle;
        ctrl_fpga_handle.reset(new electronics::component_base);
        electronics::component_base & ctrl_fpga = ctrl_fpga_handle.grab();
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
        board.add_embedded("CTRL_FPGA", ctrl_fpga_handle);
      }
      board.initialize_simple();
      crate.add_module(iboard, board_handle);
    }
    crate.tree_dump(std::cout, "Virtual FEB: ");
    crate.initialize_simple();

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
