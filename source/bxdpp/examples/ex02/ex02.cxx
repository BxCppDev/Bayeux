// Standard library
#include <iostream>
#include <exception>
#include <cstdlib>

// Third party
// - datatools
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/things.h>

// This project
#include <dpp/simple_data_sink.h>
#include <dpp/simple_brio_data_sink.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_brio_data_source.h>
#include <dpp/output_module.h>
#include <dpp/input_module.h>

int main() {
  try {

    {
      std::clog << std::endl << "Store metadata + event records..."
                << std::endl;
      dpp::simple_data_sink SDS("ex02.xml");
      SDS.open();
      for (int i(0); i < 2; i++) {
        datatools::properties header;
        header.set_description("Run header");
        header.store("Library", "Bayeux/dpp");
        header.store("Executable", "ex02");
        header.store("Value", i);
        header.store("TriggerNumber", 23);
        header.tree_dump(std::clog, "A metadata:");
        SDS.store_metadata(header);
      }
      for (int i(0); i < 5; i++) {
        datatools::things event_record;
        datatools::properties & hits = event_record.add<datatools::properties>("RawHits");
        hits.store("Hit0", "this is the first hit");
        hits.store("Hit1", "this is the second hit");
        event_record.tree_dump(std::clog, "An event record:");
        SDS.store_next_record(event_record);
      }
    }

    {
      std::clog << std::endl << "Load metadata + event records..."
                << std::endl;
      dpp::simple_data_source SDC("ex02.xml");
      SDC.open();
      for (int i = 0; i < SDC.get_number_of_metadata(); i++) {
        datatools::properties header;
        SDC.load_metadata(header, i);
        header.tree_dump(std::clog, "The loaded metadata:");
      }
      while (SDC.has_next_record()) {
        datatools::things event_record;
        SDC.load_next_record(event_record);
        event_record.tree_dump(std::clog, "The loaded event record:");
      }
    }

    {
      std::clog << std::endl << "Store metadata + event records (brio)..."
                << std::endl;
      dpp::simple_brio_data_sink SBDS("ex02.brio");
      SBDS.open();
      for (int i(0); i < 2; i++) {
        datatools::properties header;
        header.set_description("Run header");
        header.store("HeaderId", i);
        header.store("Library", "Bayeux/dpp");
        header.store("Executable", "ex02");
        header.store("Value", i);
        header.store("TriggerNumber", 23);
        header.tree_dump(std::clog, "A metadata:");
        SBDS.store_metadata(header);
      }
      for (int i(0); i < 3; i++) {
        datatools::things event_record;
        datatools::properties & hits = event_record.add<datatools::properties>("RawHits");
        hits.store("Hit0", "this is the first hit");
        hits.store("Hit1", "this is the second hit");
        event_record.tree_dump(std::clog, "An event record:");
        SBDS.store_next_record(event_record);
      }
      // As we access the storage randomly, we can add a footer:
      datatools::properties footer;
      footer.set_description("Run footer");
      footer.store("FooterId", 0);
      footer.store("Foo", 666);
      SBDS.store_metadata(footer);
    }

    {
      std::clog << std::endl << "Load metadata + event records (brio)..."
                << std::endl;
      dpp::simple_brio_data_source SBDC("ex02.brio");
      SBDC.open();
      for (int i = 0; i < SBDC.get_number_of_metadata(); i++) {
        datatools::properties header;
        SBDC.load_metadata(header, i);
        header.tree_dump(std::clog, "The loaded metadata:");
      }
      while (SBDC.has_next_record()) {
        datatools::things event_record;
        SBDC.load_next_record(event_record);
        event_record.tree_dump(std::clog, "The loaded event record:");
      }
    }

    {
      std::clog << std::endl << "Using the output module to store metadata + event records (xml)..."
                << std::endl;
      // Some parameters for the "run header":
      unsigned int number_of_events = 5;
      int trigger_number = 23;
      std::string setup_label = "MySetup";
      std::string setup_version = "2.3";
      int prng_seed = 314159;

      // The output module:
      dpp::output_module OM;
      OM.set_name("OM");
      OM.set_single_output_file("ex02b.xml");

      // Handle to the metadata store:
      datatools::multi_properties & MDS = OM.grab_metadata_store();

      // Add a "run header" section in the metadata store:
      datatools::properties & run_header = MDS.add_section("RunHeader");
      run_header.store("number_of_events", (int32_t) number_of_events);
      run_header.store("trigger_number",   trigger_number);
      run_header.store("setup.label",      setup_label);
      run_header.store("setup.version",    setup_version);
      run_header.store("prng.seed",        prng_seed);

      // Add a "miscellaneous" section in the metadata store:
      datatools::properties & misc = MDS.add_section("Misc");
      misc.store("lab", "LSM");
      misc.store("shifter", "King Arthur");
      misc.store("lunch", "cured ham");

      MDS.tree_dump(std::clog, "Metadata store:");

      // Initialize the output module:
      OM.initialize_simple();
      std::clog << "Initialization of the output module is done." << std::endl;

      std::clog << "Store event loop:" << std::endl;
      datatools::things event_record;
      for (unsigned int i(0); i < number_of_events; i++) {
        event_record.clear();
        datatools::properties & hits = event_record.add<datatools::properties>("RawHits");
        hits.store("Hit0", "this is the first hit");
        hits.store("Hit1", "this is the second hit");
        event_record.tree_dump(std::clog, "An event record:");
        dpp::base_module::process_status status = OM.process(event_record);
      }
    }

    {
      std::clog << std::endl << "Using the input module to load metadata + event records (xml)..."
                << std::endl;

      // The input module:
      dpp::input_module IM;
      IM.set_name("IM");
      IM.set_single_input_file("ex02b.xml");

      // Initialize the input module:
      IM.initialize_simple();
      std::clog << "Initialization of the input module is done." << std::endl;

      // Handle to the metadata store:
      const datatools::multi_properties & MDS = IM.get_metadata_store();

      std::clog << "Load event loop:" << std::endl;
      datatools::things event_record;
      const datatools::properties * run_header = 0;
      const datatools::properties * misc = 0;
      while (! IM.is_terminated()) {
        event_record.clear();
        dpp::base_module::process_status status = IM.process(event_record);
        if (status != dpp::base_module::PROCESS_OK) {
          break;
        }
        // Access to specific metadata sections:
        if (! run_header && MDS.has_section("RunHeader")) {
          run_header = &MDS.get_section("RunHeader");
          run_header->tree_dump(std::clog, "Metadata 'RunHeader':");
        }
        if (! misc && MDS.has_section("Misc")) {
          misc = &MDS.get_section("Misc");
          misc->tree_dump(std::clog, "Metadata 'Misc':");
        }
        event_record.tree_dump(std::clog, "An event record:");
      }
      if (! MDS.empty()) {
        MDS.tree_dump(std::clog, "Metadata store:");
      }

    }

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
