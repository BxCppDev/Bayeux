// -*- mode: c++; -*-
// test_urn_db_service.cxx

// Ourselves:
#include <datatools/urn_db_service.h>

// This project:
#include <datatools/datatools.h>

void test_urn_db_service_0();
void test_urn_db_service_1();

int main (int /* argc_ */, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  datatools::initialize();
  try {
    std::clog << "Test of the 'datatools::urn_db_service' class..." << std::endl;

    test_urn_db_service_0();
    test_urn_db_service_1();

  } catch (std::exception & x) {
    std::clog << "[fatal] " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "[fatal] " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  datatools::terminate();
  return error_code;
}

void test_urn_db_service_0()
{
  std::clog << "[info] " << "test_urn_db_service_0..." << std::endl;

  datatools::urn_db_service urnDbService;
  urnDbService.set_name("urndb");
  urnDbService.set_display_name("UrnDb");
  urnDbService.set_terse_description("URN Database Service");

  datatools::urn_info uiBxmatBasicIso;
  uiBxmatBasicIso.set_urn("urn:datatools:materials:setup:basics:1.0:isotopes");
  uiBxmatBasicIso.set_category("data");
  uiBxmatBasicIso.set_description("Bayeux/materials basic isotopes definitions version 1.0");
  urnDbService.add(uiBxmatBasicIso);

  datatools::urn_info uiSnGeoMat;
  uiSnGeoMat.set_urn("urn:snemo:demonstrator:geometry:setup:4.0:materials");
  uiSnGeoMat.set_category("configuration");
  uiSnGeoMat.set_description("Materials Plugin of the SuperNEMO Demonstrator Geometry Model version 4.0");
  uiSnGeoMat.add_component("urn:bayeux:datatools:materials:setup:basics:1.0:isotopes",  "data");
  urnDbService.add(uiSnGeoMat);

  datatools::urn_info uiSnSimuVtx;
  uiSnSimuVtx.set_urn("urn:snemo:demonstrator:simulation:vertexes:setup:4.1");
  uiSnSimuVtx.set_category("configuration");
  uiSnSimuVtx.set_description("SuperNEMO Demonstrator Simulation Vertex Generation version 4.1");
  urnDbService.add(uiSnSimuVtx);

  urnDbService.tree_dump(std::clog, urnDbService.get_name());

  std::clog << "[info] " << "End of test_urn_db_service_0." << std::endl;
  std::clog << std::endl;
  return;
}

void test_urn_db_service_1()
{
  std::clog << "[info] " << "test_urn_db_service_1..." << std::endl;

  datatools::urn_db_service urnDbService;
  urnDbService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  datatools::properties urnDbServiceConfig("Configuration of the URN DB service");
  std::vector<std::string> urn_infos_csv_filenames = {
      "${DATATOOLS_TESTING_DIR}/config/test_urn_infos_leaves.csv",
    };
  std::vector<std::string> urn_infos_defs_filenames = {
      "${DATATOOLS_TESTING_DIR}/config/test_urn_infos.defs",
      "${DATATOOLS_TESTING_DIR}/config/test_urn_infos2.defs"
    };
  urnDbServiceConfig.store("urn_infos.csv_leaves", urn_infos_csv_filenames);
  urnDbServiceConfig.store("urn_infos.definitions", urn_infos_defs_filenames);
  urnDbServiceConfig.tree_dump(std::clog, urnDbService.get_name());

  urnDbService.initialize_standalone(urnDbServiceConfig);
  urnDbService.tree_dump(std::clog, urnDbService.get_name());

  std::clog << "[info] " << "End of test_urn_db_service_1." << std::endl;
  std::clog << std::endl;
  return;
}
