/// \file datatools/test_configuration_variant_model.cxx

// Ourselves:
#include <datatools/configuration/variant_model.h>

// This project:
#include <datatools/datatools_config.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/real_range.h>

void test1()
{
  std::clog << "\nTest 1:\n";

  datatools::configuration::variant_model CV;
  CV.set_name("calibration_sources");
  CV.set_display_name("CalibSources");
  CV.set_terse_description("The SuperNEMO demonstrator variant with calibration sources");
  CV.grab_auxiliaries().store("topic", "geometry");
  CV.initialize_simple();
  CV.tree_dump(std::clog, "A configuration variant : ", "");

  return;
}

void test2()
{
  std::clog << "\nTest 2:\n";

  // Variant for the SuperNEMO demonstrator setup:
  datatools::configuration::item vSnDemoSetup;
  datatools::configuration::variant_model & sndemo_setup_CV
    = vSnDemoSetup.make_variant("snemo.demonstrator");
  sndemo_setup_CV.set_display_name("SNDemo");
  sndemo_setup_CV.set_terse_description("SuperNEMO demonstrator variant setup");

  // Create a parameter model:
  datatools::configuration::item pSNST;
  {
    datatools::configuration::parameter_model & SNST_CP
      = pSNST.make_parameter("source_thickness");
    SNST_CP.set_display_name("Source foil thickness");
    SNST_CP.set_type(datatools::TYPE_REAL);
    SNST_CP.set_terse_description("Thickness of a source foil");
    SNST_CP.set_real_unit_label("length");
    SNST_CP.set_real_preferred_unit("um");
    SNST_CP.set_real_precision(0.1 * CLHEP::micrometer);
    SNST_CP.set_variable_mode(datatools::configuration::parameter_model::VARIABLE_MODE_INTERVAL);
    datatools::real_range rdomain(10.0 * CLHEP::micrometer,
                                  100.0 * CLHEP::micrometer);
    rdomain.set_unit_label("length");
    SNST_CP.set_real_domain(rdomain);
    SNST_CP.set_default_real(50.0 * CLHEP::micrometer);
    SNST_CP.grab_auxiliaries().store("group", "geometry");
    SNST_CP.initialize_simple();
  }

  // Create another parameter model:
  datatools::configuration::item pSNSM;
  {
    datatools::configuration::parameter_model & SNSM_CP
      = pSNSM.make_parameter("source_material");
    SNSM_CP.set_display_name("Source foil material");
    SNSM_CP.set_type(datatools::TYPE_STRING);
    SNSM_CP.set_terse_description("Material of a source foil");
    SNSM_CP.set_variable_mode(datatools::configuration::parameter_model::VARIABLE_MODE_ENUM);
    SNSM_CP.add_enumerated_string_value("selenium_composite", true);
    SNSM_CP.add_enumerated_string_value("selenium_metallic");
    SNSM_CP.add_enumerated_string_value("neodynium_metallic");
    SNSM_CP.add_enumerated_string_value("calcium_composite");
    SNSM_CP.grab_auxiliaries().store("group", "geometry");
    SNSM_CP.initialize_simple();
  }

  // Add 3 parameters to the variant:
  sndemo_setup_CV.add_parameter("SrcThickness0",
                                pSNST.get_parameter_handle(),
                                "Thickness of the source foil 0");
  sndemo_setup_CV.add_parameter("SrcThickness1",
                                pSNST.get_parameter_handle(),
                                "Thickness of the source foil 1");
  sndemo_setup_CV.add_parameter("SrcThickness2",
                                pSNST.get_parameter_handle(),
                                "Thickness of the source foil 2");

  // Add 3 more parameters to the variant:
  sndemo_setup_CV.add_parameter("SrcMat0",
                                pSNSM.get_parameter_handle(),
                                "Material of the source foil 0");
  sndemo_setup_CV.add_parameter("SrcMat1",
                                pSNSM.get_parameter_handle(),
                                "Material of the source foil 1");
  sndemo_setup_CV.add_parameter("SrcMat2",
                                pSNSM.get_parameter_handle(),
                                "Material of the source foil 2");

  // Variant for the '3D' visualization view:
  datatools::configuration::item vVisu3d;
  datatools::configuration::variant_model & visu_3d_CV =
    vVisu3d.make_variant("visu.3d");
  visu_3d_CV.set_display_name("Visu3D");
  visu_3d_CV.set_terse_description("3D visualization model");
  visu_3d_CV.initialize_simple();
  visu_3d_CV.tree_dump(std::clog, "visu_3d : ", "");

  // Variant for the '2D' visualization view:
  datatools::configuration::item vVisu2d;
  datatools::configuration::variant_model & visu_2d_CV =
    vVisu2d.make_variant("visu.2d");
  visu_2d_CV.set_display_name("Visu2D");
  visu_2d_CV.set_terse_description("2D visualization model");
  visu_2d_CV.initialize_simple();
  visu_2d_CV.tree_dump(std::clog, "visu_2d : ", "");

  datatools::configuration::item pSNVV;
  {
    datatools::configuration::parameter_model & SNVV_CP
      = pSNVV.make_parameter("visu_view");
    SNVV_CP.set_display_name("Visualization view");
    SNVV_CP.set_type(datatools::TYPE_STRING);
    SNVV_CP.set_variable_mode(datatools::configuration::parameter_model::VARIABLE_MODE_ENUM);
    SNVV_CP.add_variant("view_3d", vVisu3d.get_variant_handle());
    SNVV_CP.add_variant("view_2d", vVisu2d.get_variant_handle());

    SNVV_CP.add_enumerated_string_value("3d", true);
    SNVV_CP.add_enumerated_string_value("xy");
    SNVV_CP.add_enumerated_string_value("yz");
    SNVV_CP.add_enumerated_string_value("xz");

    SNVV_CP.associate_variant_to_string("3d", "view_3d");
    SNVV_CP.associate_variant_to_string("xy", "view_2d");
    SNVV_CP.associate_variant_to_string("yz", "view_2d");
    SNVV_CP.associate_variant_to_string("xz", "view_2d");

    SNVV_CP.grab_auxiliaries().store("group", "visualization");
    SNVV_CP.initialize_simple();
  }
  sndemo_setup_CV.add_parameter("VisuView",
                                pSNVV.get_parameter_handle(),
                                "Visualization view");

  // Variant for the 'wire' visualization mode:
  datatools::configuration::item vVisuWire;
  datatools::configuration::variant_model & visu_wire_CV
    = vVisuWire.make_variant("visu.wire");
  visu_wire_CV.set_display_name("VisuWire");
  visu_wire_CV.set_terse_description("Visualization wire view");
  visu_wire_CV.initialize_simple();
  visu_wire_CV.tree_dump(std::clog, "visu_wire : ", "");

  // Variant for the 'solid' visualization mode:
  datatools::configuration::item vVisuSolid;
  datatools::configuration::variant_model & visu_solid_CV
    = vVisuSolid.make_variant("visu.solid");
  visu_solid_CV.set_display_name("VisuSolid");
  visu_solid_CV.set_terse_description("Visualization solid view");
  visu_solid_CV.initialize_simple();
  visu_solid_CV.tree_dump(std::clog, "visu_solid : ", "");

  datatools::configuration::item pSNVS;
  {
    datatools::configuration::parameter_model & SNVS_CP
      = pSNVS.make_parameter("visu_solid");
    SNVS_CP.set_display_name("Visualization solid");
    SNVS_CP.set_type(datatools::TYPE_BOOLEAN);
    SNVS_CP.set_default_boolean(false);
    SNVS_CP.add_variant("VisuWire", vVisuWire.get_variant_handle());
    SNVS_CP.add_variant("VisuSolid", vVisuSolid.get_variant_handle());
    SNVS_CP.associate_variant_to_boolean(false, "VisuWire");
    SNVS_CP.associate_variant_to_boolean(true,  "VisuSolid");
    SNVS_CP.grab_auxiliaries().store("group", "visualization");
    SNVS_CP.initialize_simple();
  }
  sndemo_setup_CV.add_parameter(pSNVS.get_name(), pSNVS.get_parameter_handle());

  sndemo_setup_CV.initialize_simple();
  sndemo_setup_CV.tree_dump(std::clog, "SN Demonstrator CV : ", "");

  return;
}

void test3()
{
  std::clog << "\nTest 3:\n";

  datatools::configuration::item_dict_type items;
  // Load a few descriptions of parameters in the dictionary of items:
  {
    datatools::configuration::parameter_model & p1 =
      datatools::configuration::item::add_parameter_item(items, "visu.view_mode.PM");
    p1.set(datatools::TYPE_STRING,
           datatools::configuration::parameter_model::VARIABLE_MODE_ENUM);
    p1.add_enumerated_string_value("3D", true);
    p1.add_enumerated_string_value("XY");
    p1.add_enumerated_string_value("XZ");

    datatools::configuration::parameter_model & p2 =
      datatools::configuration::item::add_parameter_item(items, "visu.zoom_factor.PM");
    p2.set(datatools::TYPE_REAL,
           datatools::configuration::parameter_model::VARIABLE_MODE_INTERVAL);
    p2.set_real_precision(0.005);
    datatools::real_range rdomain(1e-2, 1e2);
    p2.set_real_domain(rdomain);
    p2.set_default_real(1.0);

    datatools::configuration::parameter_model & p3 =
      datatools::configuration::item::add_parameter_item(items, "visu.textures_path.PM");
    p3.set(datatools::TYPE_STRING,
           datatools::configuration::parameter_model::VARIABLE_MODE_FREE);
    p3.set_default_string("default_textures.conf");
  }

  datatools::configuration::variant_model & visu_CV =
    datatools::configuration::item::add_variant_item(items, "visu.VM");

  datatools::properties visu_CV_setup;
  visu_CV_setup.store_string("logging.priority", "debug");
  visu_CV_setup.store_string("name", "visu");
  visu_CV_setup.store_string("display_name", "Visualization");
  visu_CV_setup.store_string("terse_description", "Configuration parameters for visualization");
  visu_CV_setup.store_string("type", "real");

  std::vector<std::string> params_labels;
  params_labels.push_back("mode");
  params_labels.push_back("zoom_factor");
  params_labels.push_back("textures_path");

  visu_CV_setup.store("parameters", params_labels);
  visu_CV_setup.store_string("parameters.mode.model",                "visu.view_mode.PM");
  visu_CV_setup.store_string("parameters.mode.description",          "Visualization view mode");
  visu_CV_setup.store_string("parameters.zoom_factor.model",         "visu.zoom_factor.PM");
  visu_CV_setup.store_string("parameters.zoom_factor.description",   "Visualization display zoom factor");
  visu_CV_setup.store_string("parameters.textures_path.model",       "visu.textures_path.PM");
  visu_CV_setup.store_string("parameters.textures_path.description", "Path to the decription of rendering textures");

  datatools::configuration::item::print_items(items, std::cout, "Dictionary of items:", "--> ");

  visu_CV.initialize(visu_CV_setup, items);
  visu_CV.tree_dump(std::clog, "A configuration variant : ");

  datatools::configuration::item::print_items(items, std::cout, "Dictionary of items:", "--> ");

  return;
}

int main (int /* argc_ */, char ** /* argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {

    test1();
    test2();
    test3();

  }
  catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...){
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
