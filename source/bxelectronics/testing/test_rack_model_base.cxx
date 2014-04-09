// Standard library
#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/rack_model_base.h>
#include <electronics/crate_model_base.h>
#include <electronics/board_model_base.h>
#include <electronics/component_types.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    // Crate model:
    electronics::crate_model_base calo_crate;
    calo_crate.set_name("CaloCrate");
    calo_crate.set_display_name("Calorimeter front end electronics crate");
    calo_crate.set_terse_description("A crate for the calorimeter front end electronics");
    calo_crate.grab_auxiliaries().store_string("model.label", "Dummy");
    calo_crate.grab_auxiliaries().store_string("model.version", "3.14");
    calo_crate.grab_auxiliaries().store_string("model.manufacturer", "ACME");
    calo_crate.set_format("VME-6U");
    calo_crate.set_max_number_of_modules(7);
    calo_crate.initialize_simple();
    calo_crate.tree_dump(std::cout, calo_crate.get_display_name()+": ");
    std::cout << std::endl;

    // Another crate model:
    electronics::crate_model_base tracker_crate;
    tracker_crate.set_name("TrackerCrate");
    tracker_crate.set_display_name("Tracker front end electronics crate");
    tracker_crate.set_terse_description("A crate for the tracker front end electronics");
    tracker_crate.grab_auxiliaries().store_string("model.label", "Foo");
    tracker_crate.grab_auxiliaries().store_string("model.version", "1.0");
    tracker_crate.grab_auxiliaries().store_string("model.manufacturer", "Tex Avery");
    tracker_crate.set_format("VME-6U");
    tracker_crate.set_max_number_of_modules(5);
    tracker_crate.initialize_simple();
    tracker_crate.tree_dump(std::cout, tracker_crate.get_display_name()+": ");
    std::cout << std::endl;

    // Rack model:
    electronics::rack_model_base rack;
    rack.set_logging_priority(datatools::logger::PRIO_TRACE);
    rack.set_name("MainRack");
    rack.set_display_name("Main front end electronics rack");
    rack.set_terse_description("The rack for all front end electronics crates");
    rack.set_format("VME-6U");
    rack.set_max_number_of_crates(4);
    rack.add_crate(0, calo_crate);    // "crate_0"
    rack.add_crate(1, calo_crate,        "CaloCrateB");
    rack.add_crate(2, tracker_crate,     "TrackoCrateA");
    rack.add_crate(3, tracker_crate); // "crate_3"
    rack.tree_dump(std::cout, "Rack: ");
    rack.remove_crate(3);
    rack.remove_embedded_component("CaloCrateB");
    rack.grab_auxiliaries().store_string("mapping.embedded.crate_0",      "[crate.ec:crate_id=0]");
    rack.grab_auxiliaries().store_string("mapping.embedded.TrackoCrateA", "[crate.ec:crate_id=2]");
    rack.initialize_simple();
    rack.tree_dump(std::cout, "Virtual rack (initialized): ");
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
