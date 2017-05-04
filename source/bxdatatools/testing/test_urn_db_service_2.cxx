// -*- mode: c++; -*-
// test_urn_db_service_2.cxx

// Ourselves:
#include <datatools/urn_db_service.h>

// This project:
#include <datatools/datatools.h>

void test_urn_db_service_0();

int main (int /* argc_ */, char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  datatools::initialize();
  try {
    std::clog << "Test of the 'datatools::urn_db_service' class..." << std::endl;

    test_urn_db_service_0();

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
  urnDbService.set_allow_mounted(false);
  urnDbService.add_allowed_category("library");
  urnDbService.add_allowed_category("application");
  urnDbService.add_allowed_category("plugin");
  urnDbService.initialize_simple();

  /*                             V
   *                            o
   *                           /
   *                          /
   *                 W     A /
   *                  o-----o
   *                       /|\
   *                      / | \
   *             U     B /  |C \ D     T
   *              o-----o   o   o-----o
   *                       / \  |
   *                      /   \ |
   *                   E /     \|F
   *                    o       o
   *                     \     / \
   *                      \   /   \
   *                       \ /     \
   *                        o       o
   *                         H       G
   *
   */

  const datatools::urn_info & uiA = urnDbService.add("urn:sw:A", "library", "A");
  const datatools::urn_info & uiB = urnDbService.add("urn:sw:B", "library", "B");
  const datatools::urn_info & uiC = urnDbService.add("urn:sw:C", "library", "C");
  const datatools::urn_info & uiD = urnDbService.add("urn:sw:D", "library", "D");
  const datatools::urn_info & uiE = urnDbService.add("urn:sw:E", "library", "E");
  const datatools::urn_info & uiF = urnDbService.add("urn:sw:F", "library", "F");
  const datatools::urn_info & uiG = urnDbService.add("urn:sw:G", "library", "G");
  const datatools::urn_info & uiH = urnDbService.add("urn:sw:H", "library", "H");
  const datatools::urn_info & uiT = urnDbService.add("urn:sw:T", "plugin",  "T");
  const datatools::urn_info & uiU = urnDbService.add("urn:sw:U", "plugin",  "U");
  const datatools::urn_info & uiV = urnDbService.add("urn:sw:V", "library", "V");
  const datatools::urn_info & uiW = urnDbService.add("urn:sw:W", "plugin",  "W");

  urnDbService.add_link(uiA.get_urn(), uiB.get_urn(), "dependency");
  urnDbService.add_link(uiA.get_urn(), uiC.get_urn(), "dependency");
  urnDbService.add_link(uiA.get_urn(), uiD.get_urn(), "dependency");
  urnDbService.add_link(uiA.get_urn(), uiW.get_urn(), "dependency");
  urnDbService.add_link(uiA.get_urn(), uiV.get_urn(), "dependency");
  urnDbService.lock();
  urnDbService.tree_dump(std::clog, urnDbService.get_name());
  std::clog << std::endl;

  datatools::urn_db_service urnDbService2;
  urnDbService2.set_name("urndb2");
  urnDbService2.set_display_name("UrnDb2");
  urnDbService2.set_terse_description("URN Database Service2");
  urnDbService2.set_allow_mounted(true);
  urnDbService2.add_allowed_category("application");
  urnDbService2.add_allowed_category("library");
  urnDbService2.add_allowed_category("plugin");
  urnDbService2.connect_db(urnDbService);
  urnDbService2.initialize_simple();
  const datatools::urn_info & uiX = urnDbService2.add("urn:sw:X", "library", "X");
  const datatools::urn_info & uiY = urnDbService2.add("urn:sw:Y", "application", "Y");
  urnDbService2.add_mounted("urndb", uiH.get_urn());
  urnDbService2.add_link(uiX.get_urn(), uiH.get_urn(), "dependency");
  urnDbService2.add_link(uiY.get_urn(), uiH.get_urn(), "dependency");
  urnDbService2.add_link(uiY.get_urn(), uiX.get_urn(), "dependency");
  urnDbService2.lock();
  urnDbService2.tree_dump(std::clog, urnDbService2.get_name());
  std::clog << std::endl;

  std::clog << "[info] " << "End of test_urn_db_service_0." << std::endl;
  std::clog << std::endl;
  return;
}
