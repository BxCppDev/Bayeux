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

    // FEB board model:
    electronics::board_model_base feb_board;
    feb_board.set_name("FEB");
    feb_board.set_display_name("Front end board");
    feb_board.set_terse_description("The frontend board model");
    feb_board.set_slot_width(1);
    feb_board.set_format("VME-6U");
    feb_board.set_max_number_of_mezzanine_boards(0);
    feb_board.initialize_simple();
    feb_board.tree_dump(std::cout, feb_board.get_display_name() + ":");
    std::cout << std::endl;

    // Control board model:
    electronics::board_model_base ctrl_board;
    ctrl_board.set_name("CB");
    ctrl_board.set_display_name("Control board");
    ctrl_board.set_terse_description("The control board model");
    ctrl_board.set_slot_width(2);
    ctrl_board.set_format("VME-6U");
    ctrl_board.set_max_number_of_mezzanine_boards(0);
    ctrl_board.initialize_simple();
    ctrl_board.tree_dump(std::cout, ctrl_board.get_display_name() + ":");
    std::cout << std::endl;

    // Crate model:
    electronics::crate_model_base crate;
    crate.set_name("Crate");
    crate.set_display_name("Front end electronic crate");
    crate.set_terse_description("A dummy crate");
    crate.set_format("VME-6U");
    crate.set_max_number_of_modules(8);
    crate.add_module(0, feb_board, "FEB0");
    crate.add_module(1, feb_board, "FEB1");
    crate.add_module(2, feb_board, "FEB2");
    crate.add_module(3, feb_board, "FEB3");
    crate.add_module(4, feb_board);
    crate.add_module(6, ctrl_board, "CB");
    crate.tree_dump(std::cout, crate.get_display_name() + ":");
    std::cout << std::endl;
    crate.remove_module(3);
    crate.remove_embedded_component("module_4");
    crate.initialize_simple();
    crate.tree_dump(std::cout, crate.get_display_name() + ":");
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
