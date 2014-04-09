#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/board_model_base.h>
#include "electronics/component_types.h"

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    // Control FPGA model (as a generic component model):
    electronics::component_model_base ctrl_fpga;
    ctrl_fpga.set_type(electronics::TYPE_CHIP_PLD);
    ctrl_fpga.set_name("CTRL_FPGA");
    ctrl_fpga.set_display_name("Control FPGA");
    ctrl_fpga.set_terse_description("The control FPGA model for the front end board");
    ctrl_fpga.grab_auxiliaries().store_string("manufacturer", "ALTERA");
    ctrl_fpga.grab_auxiliaries().store_string("model.label", "Cyclone");
    ctrl_fpga.grab_auxiliaries().store_string("model.version", "II");
    ctrl_fpga.grab_auxiliaries().store_string("firmware.label", "CTRLsys");
    ctrl_fpga.grab_auxiliaries().store_string("firmware.version", "1.0");
    ctrl_fpga.initialize_simple();
    ctrl_fpga.tree_dump(std::cout, ctrl_fpga.get_display_name() + ":");
    std::cout << std::endl;

    // FEB FPGA model (as a generic component model):
    electronics::component_model_base feb_fpga;
    feb_fpga.set_type(electronics::TYPE_CHIP_PLD);
    feb_fpga.set_name("FEB_FPGA");
    feb_fpga.set_display_name("FEB FPGA");
    feb_fpga.set_terse_description("The FEB FPGA model for the front end board");
    feb_fpga.grab_auxiliaries().store_string("manufacturer", "ALTERA");
    feb_fpga.grab_auxiliaries().store_string("model.label", "Cyclone");
    feb_fpga.grab_auxiliaries().store_string("model.version", "II");
    feb_fpga.grab_auxiliaries().store_string("firmware.label", "FEBsys");
    feb_fpga.grab_auxiliaries().store_string("firmware.version", "1.3");
    feb_fpga.initialize_simple();
    feb_fpga.tree_dump(std::cout, feb_fpga.get_display_name() + ":");
    std::cout << std::endl;

    // Mezzanine board:
    electronics::board_model_base mezzanine_board;
    mezzanine_board.set_name("FEBMez");
    mezzanine_board.set_display_name("FEB mezzanine board");
    mezzanine_board.set_terse_description("A mezzanine board model for the front end board");
    mezzanine_board.set_type(electronics::TYPE_MODULE_MEZZANINE_BOARD);
    mezzanine_board.grab_auxiliaries().store_string("manufacturer", "ACME");
    mezzanine_board.set_format("mezz");
    mezzanine_board.initialize_simple();
    mezzanine_board.tree_dump(std::cout, mezzanine_board.get_display_name() + ":");
    std::cout << std::endl;

    // Board:
    electronics::board_model_base board;
    board.set_name("FEB");
    board.set_display_name("Front end board");
    board.set_terse_description("The frontend board model");
    board.grab_auxiliaries().store_string("model.label", "Dummy");
    board.grab_auxiliaries().store_string("model.version", "1");
    board.grab_auxiliaries().store_string("manufacturer", "ACME");
    board.set_slot_width(1);
    board.set_format("VME-6U");
    board.set_mezzanine_format("mezz");
    board.set_max_number_of_mezzanine_boards(6);
    board.add_mezzanine_board(0, mezzanine_board);
    board.add_mezzanine_board(1, mezzanine_board);
    board.add_mezzanine_board(2, mezzanine_board);
    board.add_mezzanine_board(4, mezzanine_board);
    board.add_embedded_component("CTRL_FPGA", ctrl_fpga);
    board.add_embedded_component(electronics::make_indexed_name("FEB_FPGA_", 0), feb_fpga);
    board.add_embedded_component(electronics::make_indexed_name("FEB_FPGA_", 1), feb_fpga);
    board.tree_dump(std::cout, board.get_display_name() + ":");
    std::cout << std::endl;
    board.remove_mezzanine_board(2);
    board.remove_embedded_component("mezzanine_4");
    board.initialize_simple();
    board.tree_dump(std::cout, board.get_display_name() + ":");
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
