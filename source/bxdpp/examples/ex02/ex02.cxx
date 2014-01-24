// Standard library
#include <iostream>
#include <exception>
#include <cstdlib>

// Third party
// - datatools
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/things.h>
#include <datatools/service_manager.h>

// This project
#include <dpp/simple_data_sink.h>
#include <dpp/simple_brio_data_sink.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_brio_data_source.h>
#include <dpp/output_module.h>
#include <dpp/input_module.h>
#include <dpp/context_service.h>

void test1w()   {
  std::clog << std::endl << "Store metadata + event records (xml)..."
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
  return;
}

void test1r()  {
  std::clog << std::endl << "Load metadata + event records (xml)..."
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
  return;
}

void test2w()   {
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
  return;
}

void test2r()   {
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
  return;
}

void test3w() {
  std::clog << std::endl << "Using the output module to store embedded metadata + event records (xml)..."
            << std::endl;
  // Some parameters for the "run header" (metadata):
  unsigned int number_of_events = 8;
  int          trigger_number = 23;
  std::string  setup_label = "MySetup";
  std::string  setup_version = "2.3";
  int          prng_seed = 314159;

  // The output module:
  dpp::output_module OM;
  OM.set_logging_priority(datatools::logger::PRIO_FATAL);
  OM.set_name("OM");
  OM.set_description("Example output module");
  OM.set_preserve_existing_output(false); // Allowed to erase existing output file
  //OM.set_single_output_file("ex02b.xml");
  // Multi-file mode:
  OM.set_incremental_output_files("", "ex02b_", ".xml", 10);
  OM.set_limits(0, 3, 0); // max. 3 event records per file

  // Handle to the metadata store embedded in the output module:
  datatools::multi_properties & MDS = OM.grab_metadata_store();
  // Add a "run header" section in the metadata store:
  datatools::properties & run_header = MDS.add_section("RunHeader");
  run_header.set_description("Run header metadata");
  run_header.store("number_of_events", (int32_t) number_of_events);
  run_header.store("trigger_number",   trigger_number);
  run_header.store("setup.label",      setup_label);
  run_header.store("setup.version",    setup_version);
  run_header.store("prng.seed",        prng_seed);
  // Add a "miscellaneous" section in the metadata store:
  datatools::properties & misc = MDS.add_section("Misc");
  misc.set_description("Miscellaneous metadata");
  misc.store("lab", "LSM");
  misc.store("shifter", "King Arthur");
  misc.store("lunch", "cured ham");
  // Print:
  MDS.tree_dump(std::clog, "Metadata store (before initialization):");

  // Initialize the output module:
  OM.initialize_simple();
  std::clog << "Initialization of the output module is done." << std::endl;

  // Event loop:
  std::clog << "Store event loop:" << std::endl;
  datatools::things event_record;
  for (unsigned int i(0); i < number_of_events; i++) {
    std::clog << " - Event loop #" << i << std::endl;
    event_record.clear();
    std::ostringstream namess;
    namess << "ER_" << i;
    event_record.set_name(namess.str());
    event_record.set_description("A mock event record");
    datatools::properties & hits = event_record.add<datatools::properties>("RawHits");
    hits.store("Hit0", "this is the first hit");
    hits.store("Hit1", "this is the second hit");
    event_record.tree_dump(std::clog, "An event record:");
    dpp::base_module::process_status status = OM.process(event_record);
    if (status != dpp::base_module::PROCESS_OK) {
      std::cerr << "Could not store the event record #" << i << " !" << std::endl;
      break;
    }
  }
  std::clog << "End of store event loop." << std::endl;
  return;
}

void test3r(){
  std::clog << std::endl << "Using the input module to load embedded metadata + event records (xml)..."
            << std::endl;

  // The input module:
  dpp::input_module IM;
  // IM.set_logging_priority(datatools::logger::PRIO_TRACE);
  IM.set_name("IM");
  IM.set_description("Example input module");
  // IM.set_single_input_file("ex02b.xml");
  IM.set_incremental_input_files("", "ex02b_", ".xml", -1);
  // Initialize the input module:
  IM.initialize_simple();
  IM.tree_dump(std::clog, "Input module (after initialization):");
  std::clog << "Initialization of the input module is done." << std::endl;

  // Handle to the metadata store:
  const datatools::multi_properties & MDS = IM.get_metadata_store();
  MDS.tree_dump(std::clog, "Metadata store (before event loop):");
  // Access to specific metadata sections:
  if (MDS.has_section("RunHeader")) {
    const datatools::properties & run_header = MDS.get_section("RunHeader");
    run_header.tree_dump(std::clog, "Metadata 'RunHeader':");
  }
  if (MDS.has_section("Misc")) {
    const datatools::properties & misc = MDS.get_section("Misc");
    misc.tree_dump(std::clog, "Metadata 'Misc':");
  }

  std::clog << "Load event loop:" << std::endl;
  datatools::things event_record;
  unsigned int counter = 0;
  while (! IM.is_terminated()) {
    std::clog << " - Event loop #" << counter << std::endl;
    event_record.clear();
    dpp::base_module::process_status status = IM.process(event_record);
    if (status != dpp::base_module::PROCESS_OK) {
      std::clog << "INFO: Cannot process another event record !"  << std::endl;
      break;
    }
    if (IM.metadata_was_updated()) {
      std::clog << " - Metadata data was updated !"  << std::endl;
      MDS.tree_dump(std::clog, "Metadata store (after event loading):");
    }
    event_record.tree_dump(std::clog, "An event record:");
    counter++;
  }
  std::clog << "End of load event loop." << std::endl;
  if (! MDS.empty()) {
    MDS.tree_dump(std::clog, "Metadata store (after event loop):");
  }
  // Reset the input module:
  IM.reset();
  return;
}

void test4w() {
  std::clog << std::endl << "Using the output module to store context + embedded metadata + event records (xml)..."
            << std::endl;

  // ********* The service manager *********
  datatools::service_manager SM;
  datatools::properties ctx_config;
  SM.load("Ctx", "dpp::context_service", ctx_config); // Add a context service named 'Ctx'

  // ********* Access to the context service *********
  dpp::context_service & CS = SM.grab<dpp::context_service>("Ctx");
  // Some parameters for the global metadata:
  std::string operator_name = "jeanlou";
  std::string operator_group = "shifter";
  int32_t     run_date = 1390397890;
  // Handle to the global metadata store:
  datatools::multi_properties & GD = CS.grab_store();
  // Add a "Global" section in the metadata store:
  datatools::properties & global = GD.add_section("Global");
  global.store("operator_name", operator_name);
  global.store("operator_group", operator_group);
  global.store("run_date", run_date);
  global.tree_dump(std::clog, "Global metadata store in 'Ctx':");
  // Add a "_Transient_" section in the metadata store:
  datatools::properties & transient = GD.add_section("_Transient_");
  transient.store("dummy_param", "foo");
  transient.tree_dump(std::clog, "Transient metadata store in 'Ctx':");

  // ********* The output module *********
  dpp::output_module OM;
  OM.set_name("OM");
  OM.set_description("Example output module with metadata from context service");
  OM.set_context_label("Ctx"); // Set the name of the context service to be used
  // OM.set_export_context_all(true); // Export all metadata sections from the context service to the output stream
  OM.export_context_metadata("Global"); // Export a metadata section with a given name from the context service to the output stream
  // OM.set_single_output_file("ex02c.xml");
  // Multi-file mode:
  OM.set_incremental_output_files("", "ex02c_", ".xml", 10);
  OM.set_limits(0, 3, 0); // max. 3 event records per file

  // Some parameters for the "run header" metadata:
  unsigned int number_of_events = 4;
  int trigger_number = 23;
  std::string setup_label = "MySetup";
  std::string setup_version = "2.3";
  int prng_seed = 314159;

  // Handle to the metadata store embedded to the I/O module:
  datatools::multi_properties & MDS = OM.grab_metadata_store();
  // Add a "run header" section in the metadata store:
  datatools::properties & run_header = MDS.add_section("RunHeader");
  run_header.store_integer("number_of_events", (int32_t) number_of_events);
  run_header.store_integer("trigger_number",   (int32_t) trigger_number);
  run_header.store_string("setup.label",       setup_label);
  run_header.store_string("setup.version",     setup_version);
  run_header.store_integer("prng.seed",        (int32_t) prng_seed);
  // Add a "miscellaneous" section in the metadata store:
  datatools::properties & misc = MDS.add_section("Misc");
  misc.store("lab", "LSM");
  misc.store("shifter", "King Arthur");
  misc.store("lunch", "cured ham");
  // Print:
  MDS.tree_dump(std::clog, "Metadata store:");

  // Initialize the output module:
  datatools::properties outputModuleConf;
  OM.initialize_with_service(outputModuleConf, SM);
  std::clog << "Initialization of the output module is done." << std::endl;

  // Event loop:
  std::clog << "Store event loop:" << std::endl;
  datatools::things event_record;
  for (unsigned int i(0); i < number_of_events; i++) {
    std::clog << " - Event loop #" << i << std::endl;
    event_record.clear();
    std::ostringstream namess;
    namess << "ER_" << i;
    event_record.set_name(namess.str());
    event_record.set_description("A mock event record");
    datatools::properties & hits = event_record.add<datatools::properties>("RawHits");
    hits.store_integer("EventId", (int32_t) i);
    hits.store_string("Hit0", "this is the first hit");
    hits.store_string("Hit1", "this is the second hit");
    event_record.tree_dump(std::clog, "An event record:");
    dpp::base_module::process_status status = OM.process(event_record);
    if (status != dpp::base_module::PROCESS_OK) {
      std::cerr << "Could not store the event record #" << i << " !" << std::endl;
      break;
    }
  }
  return;
}

void test4r(){
  std::clog << std::endl << "Using the input module to load context + embedded metadata + event records (xml)..."
            << std::endl;

  // ********* The service manager *********
  datatools::service_manager SM;
  datatools::properties ctx_config;
  SM.load("Ctx", "dpp::context_service", ctx_config); // Add a context service named 'Ctx'

  // ********* The input module *********
  dpp::input_module IM;
  //IM.set_logging_priority(datatools::logger::PRIO_TRACE);
  IM.set_name("IM");
  IM.set_description("Example input module");
  IM.set_context_label("Ctx"); // Set the name of the context service to be used
  // IM.import_context_metadata("Global"); // Import a metadata section with given name from the intput stream to the context service
  IM.set_import_context_all(true); // Import all metadata sections from the intput stream to the context service
  // IM.set_single_input_file("ex02c.xml");
  IM.set_incremental_input_files("", "ex02c_", ".xml", -1);
  // Initialize the input module:
  datatools::properties inputModuleConf;
  IM.initialize_with_service(inputModuleConf, SM);
  std::clog << "Initialization of the input module is done." << std::endl;
  IM.tree_dump(std::clog, "Input module (after initialization):");

  // ********* Access to the context service *********
  const dpp::context_service & CS = SM.get<dpp::context_service>("Ctx");
  const datatools::multi_properties & GD = CS.get_store();
  GD.tree_dump(std::clog, "Context service store (loaded from the input module):");
  if (GD.has_section("Global")) {
    const datatools::properties & global = GD.get_section("Global");
    global.tree_dump(std::clog, "Global metadata store in 'Ctx' (loaded from the input module):");
  } else {
    std::clog << "There is no context metadata with label 'Global' !" << std::endl;
  }

  // Handle to the metadata store:
  const datatools::multi_properties & MDS = IM.get_metadata_store();
  MDS.tree_dump(std::clog, "Metadata store (loaded from the input module before event loop):");
  // Access to specific metadata sections:
  if (MDS.has_section("RunHeader")) {
    const datatools::properties & run_header = MDS.get_section("RunHeader");
    run_header.tree_dump(std::clog, "Metadata 'RunHeader':");
  }
  if (MDS.has_section("Misc")) {
    const datatools::properties & misc = MDS.get_section("Misc");
    misc.tree_dump(std::clog, "Metadata 'Misc':");
  }

  std::clog << "Load event loop:" << std::endl;
  datatools::things event_record;
  unsigned int counter = 0;
  while (! IM.is_terminated()) {
    std::clog << " - Event loop #" << counter << std::endl;
    event_record.clear();
    dpp::base_module::process_status status = IM.process(event_record);
    if (status != dpp::base_module::PROCESS_OK) {
      std::clog << "INFO: Cannot process another event record !"  << std::endl;
      break;
    }
    if (IM.metadata_was_updated()) {
      std::clog << " - Metadata data was updated !"  << std::endl;
      MDS.tree_dump(std::clog, "Metadata store (after event loading):");
    }
    event_record.tree_dump(std::clog, "An event record:");
    counter++;
  }
  std::clog << "End of load event loop." << std::endl;
  if (! MDS.empty()) {
    MDS.tree_dump(std::clog, "Metadata store (after event loop):");
  }
  // Reset the input module:
  IM.reset();
  return;
}

int main() {
  try {

    test1w();
    test1r();

    test2w();
    test2r();

    test3w();
    test3r();

    test4w();
    test4r();

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
