/// \file datatools/test_configuration_variant_dependency.cxx

// Ourselves:
#include <datatools/configuration/variant_dependency.h>

// Standard library:
#include <cstdlib>
#include <iostream>

// - Bayeux:
#include <bayeux/bayeux.h>

// This project:
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/ui/variant_repository_cli.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_registry_manager.h>
#include <datatools/configuration/variant_dependency_model.h>
#include <datatools/configuration/variant_dependency_logic.h>
#include <datatools/configuration/variant_dependency_logic_parsing.h>

void test_logic_parsing_ast();
void test0();

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {

   test_logic_parsing_ast();

   test0();

  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
    error_code = EXIT_FAILURE;
  } catch (...){
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}

void test_logic_parsing_ast()
{
  std::clog << "\nTest logic parsing AST:\n";

  std::vector<std::string> formulas =
    {
      "12",
      "[0]",
      "NOT([0])",
      "AND([0],[2])",
      "AND(NOT([0]), OR([3], [12]))",
      "xor(1,or(15,18),5,[7], AND(NOT(0), OR([3], 12)))",
      "foo",
      "-12",
      "and(12)",
      "not(12)",
      "not(12,3)",
    };
  for (const std::string & logic_formula : formulas) {
    datatools::configuration::parsing::dependency_logic_ast ast;

    if (!ast.parse(logic_formula)) {
      std::cerr << "error: Invalid formula   : '" << logic_formula << "'!" << std::endl;
    } else {
      std::clog << "info: Formula is correct : '" << logic_formula << "'" << std::endl;
      ast.print(std::clog);
    }
    std::clog << std::endl;
  }

  return;
}

void test0()
{
  std::clog << "\nTest 0:\n";

  datatools::configuration::variant_registry_manager mgr;
  datatools::properties mgr_config;
  std::string mgr_config_filename =
    "${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_registry_manager.conf";
  datatools::fetch_path_with_env(mgr_config_filename);
  datatools::read_config(mgr_config_filename, mgr_config);
  mgr.initialize(mgr_config);
  mgr.tree_dump(std::clog, "Variant manager:");
  std::clog << std::endl;

  datatools::configuration::variant_registry radioactivity_vreg;
  radioactivity_vreg.set_name("radioactivity");
  radioactivity_vreg.set_display_name("Radioactivity");
  radioactivity_vreg.set_terse_description("Configuration variants for the setup radioactivity");
  radioactivity_vreg.initialize_from(mgr, "if_radioactive.VM");

  datatools::configuration::variant_repository vrep;
  vrep.set_name("my_experiment");
  vrep.set_display_name("My experiment");
  vrep.set_terse_description("The repository for my experiment configuration parameters");
  vrep.registration_external(radioactivity_vreg,
                             "radioactivity"
                             );
  vrep.registration_embedded("${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_registry_manager.conf",
                             "geometry.VM",
                             "geometry"
                             );
  vrep.registration_embedded("${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_registry_manager_2.conf",
                             "vertex_generation.VM",
                             "vertexes", /* Choose a registration name */
                             "Vertex generation", /* Choose a registration display name */
                             "Configuration for the vertex generation" /* Terse description */
                             );

  // Command line interface tool for a variant repository:
  datatools::configuration::ui::variant_repository_cli vrepCli(vrep);
  vrepCli.cmd_set_parameter("geometry", "has_detector_0", "1");
  vrepCli.cmd_set_parameter("geometry", "has_detector_1", "1");
  vrepCli.cmd_set_parameter("geometry", "has_detector_1/if_detector/length", "1 mm");
  vrepCli.cmd_set_parameter("geometry", "has_detector_0/if_detector/material", "Germanium");
  vrepCli.cmd_set_parameter("geometry", "firstname", "Betty");
  vrepCli.cmd_set_parameter("geometry", "firstname", "John");

  vrep.tree_dump(std::clog, "The variant repository: ", "[info] ");
  std::clog << std::endl;

  bool test1 = true;
  // test1 = false;
  if (test1) {
    std::clog << "\nTest 0: test1\n";
    datatools::configuration::variant_dependency vdep(vrep);
    vdep.set_logging(datatools::logger::PRIO_DEBUG);
    // Set the depender:
    vdep.set_depender_by_path("geometry:has_detector_0/if_detector/material/if_radioactive/isotope@Background");
    // Set the dependees:
    auto if_detector0_slot = vdep.add_dependee_by_path("geometry:has_detector_0/if_detector");
    std::clog << "[info] if_detector0_slot = [#" << if_detector0_slot << "]" << std::endl;
    auto if_detector1_slot = vdep.add_dependee_by_path("geometry:has_detector_1/if_detector");
    std::clog << "[info] if_detector1_slot = [#" << if_detector1_slot << "]" << std::endl;
    auto if_betty_slot     = vdep.add_dependee_by_path("geometry:firstname/if_betty");
    std::clog << "[info] if_betty_slot     = [#" << if_betty_slot << "]" << std::endl;
    // vdep.add_dependee_by_path("cabling:layout/is_half_commissioning/color[12]"); // This *must* fail
    vdep.tree_dump(std::clog, "Variant dependency (without logic): ", "[info] ");

    // Define the logic:
    //
    //   <-- AND (top_logic)
    //       |-- OR
    //       |   |-- SLOT[0] <-- (if_detector0_slot)
    //       |   `-- SLOT[1] <-- (if_detector1_slot)
    //       `-- NOT
    //           `-- SLOT[2] <-- (if_betty_slot)
    //
    auto & top_logic = vdep.create_logic("and");
    auto & or_logic  = top_logic.connect(0, "or");
    or_logic.connect_dependee(0, if_detector0_slot);
    or_logic.connect_dependee(1, if_detector1_slot);
    auto & not_logic = top_logic.connect(1, "not");
    not_logic.connect_dependee(0, if_betty_slot);
    // Lock the logic:
    vdep.lock();

    vdep.tree_dump(std::clog, "Variant dependency (with logic): ");
    if (vdep()) {
      std::clog << "Dependency is fulfilled!" << std::endl;
    } else {
      std::clog << "Dependency is NOT fulfilled!" << std::endl;
    }
    std::clog << std::endl;
  }

  bool test2 = true;
  // test2 = false;
  if (test2) {
    std::clog << "\nTest 0: test2\n";
    datatools::configuration::variant_dependency vdep(vrep);
    // Set the depender:
    vdep.set_depender_by_path("vertexes:generator@Test");

    // Set the dependees:
    auto is_half_commissioning_slot = vdep.add_dependee_by_path("geometry:has_detector_0/if_detector");
    auto is_betty_slot = vdep.add_dependee_by_path("geometry:firstname/if_betty");
    // auto is_rainy_slot = vdep.add_dependee_by_path("electronics:weather/is_rainy");
    // auto is_pro_slot = vdep.add_dependee_by_path("cabling:layout/is_half_commissioning/version/is_pro");
    std::clog << "[Info] 'HC'    dependee slot: " << is_half_commissioning_slot << std::endl;
    std::clog << "[Info] 'betty' dependee slot: " << is_betty_slot << std::endl;
    // std::clog << "[Info] 'rainy' dependee slot: " << is_rainy_slot << std::endl;
    // std::clog << "[Info] 'pro'   dependee slot: " << is_pro_slot << std::endl;

    // Lock with default logic:
    vdep.lock();
    vdep.tree_dump(std::clog, "Variant dependency: ");
    std::clog << std::endl;
  }

  bool test3 = true;
  // test3 = false;
  if (test3) {
    std::clog << "\nTest 0: test3\n";
    datatools::configuration::variant_dependency_model vdepmod(vrep);
    datatools::properties vdepmod_config;
    std::string vdepmod_config_filename =
      "${DATATOOLS_TESTING_DIR}/config/vdepmod.conf";
    datatools::fetch_path_with_env(vdepmod_config_filename);
    datatools::read_config(vdepmod_config_filename, vdepmod_config);
    vdepmod.initialize(vdepmod_config);
    vdepmod.tree_dump(std::clog, "Variant dependency model: ");
    std::clog << std::endl;
  }

  bool test4 = true;
  // test4 = false;
  if (test4) {
    std::clog << "\nTest 0: test4\n";
    datatools::configuration::variant_dependency_model vdepmod(vrep);
    datatools::properties vdepmod_config;
    std::string vdepmod_config_filename =
      "${DATATOOLS_TESTING_DIR}/config/vdepmod2.conf";
    datatools::fetch_path_with_env(vdepmod_config_filename);
    datatools::read_config(vdepmod_config_filename, vdepmod_config);
    vdepmod.initialize(vdepmod_config);
    vdepmod.tree_dump(std::clog, "Variant dependency model: ");
    std::clog << std::endl;
  }

  std::clog << std::endl;
  return;
}
