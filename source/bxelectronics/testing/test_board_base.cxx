#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/board_base.h>
#include "electronics/component_types.h"

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    // Board:
    electronics::component_handle_type board_handle;

    board_handle.reset(new electronics::board_base);
    electronics::board_base & board = dynamic_cast<electronics::board_base &>(board_handle.grab());
    board.set_name(electronics::make_indexed_name("FEB_", 0));
    board.set_display_name(electronics::make_indexed_name("FEB #", 0));
    board.set_terse_description("A dummy frontend board");
    board.grab_auxiliaries().store_string("model.label", "Dummy");
    board.grab_auxiliaries().store_string("model.version", "1");
    board.grab_auxiliaries().store_string("manufacturer", "ACME");
    board.grab_auxiliaries().store_string("serial_number", "00102030");
    board.grab_auxiliaries().store_string("inventory_number", "CNRS-999-777-3232");
    board.set_slot_width(1);
    board.set_format("VME-6U");
    board.set_mezzanine_format("VME-6U-mezz");
    board.set_max_number_of_mezzanine_boards(4);

    for (int imezz = 0; imezz < 4; imezz++) {
      if (imezz == 1) continue;
      // Mezzanine:
      electronics::component_handle_type mezzanine_handle;
      mezzanine_handle.reset(new electronics::board_base);
      electronics::board_base & mezzanine =
        dynamic_cast<electronics::board_base &>(mezzanine_handle.grab());
      mezzanine.set_name(electronics::make_indexed_name("Mezzanine_", imezz));
      mezzanine.set_display_name(electronics::make_indexed_name("Mezzanine board #", imezz));
      mezzanine.set_terse_description("Mezzanine board");
      mezzanine.set_type(electronics::TYPE_MODULE_MEZZANINE_BOARD);
      mezzanine.grab_auxiliaries().store_string("manufacturer", "ACME");
      mezzanine.grab_auxiliaries().store_string("serial_number", "?");
      mezzanine.set_format("VME-6U-mezz");
      mezzanine.tree_dump(std::cout, "Mezzanine board: ");
      mezzanine.initialize_simple();

      board.add_mezzanine_board(imezz, mezzanine_handle);
    }

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

    board.tree_dump(std::cout, "Virtual FEB: ");
    board.remove_mezzanine_board(2);
    board.tree_dump(std::cout, "Virtual FEB: ");
    board.initialize_simple();

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
