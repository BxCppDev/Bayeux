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

    // Board:
    electronics::component_handle_type board_handle;
    board_handle.reset(new electronics::component_model_base);
    electronics::component_model_base & board = board_handle.grab();
    board.set_name(electronics::make_indexed_name("Calo_FEB_", 0));
    board.set_display_name(electronics::make_indexed_name("Calo FEB #", 0));
    board.set_terse_description("SuperNEMO calorimeter 8-channel WaveCatcher frontend board");
    board.set_type(electronics::TYPE_MODULE_BOARD);
    board.grab_auxiliaries().store_string("model.label", "WAVECAT_16CH");
    board.grab_auxiliaries().store_string("model.version", "2");
    board.grab_auxiliaries().store_string("manufacturer", "LAL/Orsay");
    board.grab_auxiliaries().store_string("serial_number", "1110");
    board.grab_auxiliaries().store_string("inventory_number", "CNRS-123-456-7890");

    for (int i_asic = 0; i_asic < 8; i_asic++) {
      // ASIC:
      electronics::component_handle_type asic_handle;
      asic_handle.reset(new electronics::component_model_base);
      electronics::component_model_base & asic = asic_handle.grab();
      asic.set_name(electronics::make_indexed_name("Calo_ASIC_", i_asic));
      asic.set_display_name(electronics::make_indexed_name("Calo ASIC #", i_asic));
      asic.set_terse_description("2-channel SAMLONG chip");
      asic.set_type(electronics::TYPE_CHIP_ASIC);
      asic.grab_auxiliaries().store_string("model.label", "SAMLONG_1024");
      asic.grab_auxiliaries().store_string("model.version", "1");
      asic.grab_auxiliaries().store_string("manufacturer", "LAL/Orsay");
      asic.grab_auxiliaries().store_string("serial_number", "12345");
      asic.tree_dump(std::cout, "Electronic component (ASIC): ");
      asic.initialize_simple();

      board.add_embedded(electronics::make_indexed_name("ASIC_", i_asic), asic_handle);
    }

    for (int i_fe_fpga = 0; i_fe_fpga < 4; i_fe_fpga++) {
      // Front end FPGA:
      electronics::component_handle_type fe_fpga_handle;
      fe_fpga_handle.reset(new electronics::component_model_base);
      electronics::component_model_base & fe_fpga = fe_fpga_handle.grab();
      fe_fpga.set_name(electronics::make_indexed_name("Calo_FE_FPGA_", i_fe_fpga));
      fe_fpga.set_display_name(electronics::make_indexed_name("Calo FE FPGA #", i_fe_fpga));
      fe_fpga.set_terse_description("SuperNEMO calorimeter 4-channel FE_FPGA");
      fe_fpga.set_type(electronics::TYPE_CHIP_PLD);
      fe_fpga.grab_auxiliaries().store_string("manufacturer", "ALTERA");
      fe_fpga.grab_auxiliaries().store_string("serial_number", "?");
      fe_fpga.grab_auxiliaries().store_string("model.label", "Cyclone");
      fe_fpga.grab_auxiliaries().store_string("model.version", "II");
      fe_fpga.grab_auxiliaries().store_string("firmware.label", "SNEMO_FE");
      fe_fpga.grab_auxiliaries().store_string("firmware.version", "1.0");
      fe_fpga.tree_dump(std::cout, "Calo_FE_FPGA: ");
      fe_fpga.initialize_simple();

      board.add_embedded(electronics::make_indexed_name("FE_FPGA_", i_fe_fpga), fe_fpga_handle);
    }

    {
      // Control FPGA:
      electronics::component_handle_type ctrl_fpga_handle;
      ctrl_fpga_handle.reset(new electronics::component_model_base);
      electronics::component_model_base & ctrl_fpga = ctrl_fpga_handle.grab();
      ctrl_fpga.set_name("Calo_CTRL_FPGA");
      ctrl_fpga.set_display_name("Calo FE FPGA");
      ctrl_fpga.set_terse_description("SuperNEMO calorimeter 16-channel CTRL_FPGA");
      ctrl_fpga.set_type(electronics::TYPE_CHIP_PLD);
      ctrl_fpga.grab_auxiliaries().store_string("manufacturer", "ALTERA");
      ctrl_fpga.grab_auxiliaries().store_string("serial_number", "?");
      ctrl_fpga.grab_auxiliaries().store_string("model.label", "Cyclone");
      ctrl_fpga.grab_auxiliaries().store_string("model.version", "II");
      ctrl_fpga.grab_auxiliaries().store_string("firmware.label", "SNEMO_CTRL");
      ctrl_fpga.grab_auxiliaries().store_string("firmware.version", "1.0");
      ctrl_fpga.tree_dump(std::cout, "Calo_CTRL_FPGA: ");
      ctrl_fpga.initialize_simple();

      board.add_embedded("CTRL_FPGA", ctrl_fpga_handle);
    }
    board.tree_dump(std::cout, "Virtual CaloFEB: ");

    // Remove one ASIC embedded:
    board.remove_embedded(electronics::make_indexed_name("ASIC_", 2));

    // Smart print:
    board.initialize_simple();
    board.tree_dump(std::cout, "Virtual CaloFEB (minus one ASIC): ");

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
