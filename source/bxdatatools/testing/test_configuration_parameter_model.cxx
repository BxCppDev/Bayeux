/// \file datatools/test_configuration_parameter_model.cxx

// - Ourselves
#include <datatools/configuration/parameter_model.h>

// - Boost
// #include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/datatools_config.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/real_range.h>
#include <datatools/configuration/utils.h>
#include <datatools/configuration/variant_model.h>

void test1()
{
  std::clog << "\nTest 1:\n";

  // Create 3 dummy variant models:
  datatools::configuration::item iCV1;
  datatools::configuration::variant_model & CV1 = iCV1.make_variant("small_foil_thickness");
  CV1.set_documentation("Variant model associated to small values of the source foil thickness");

  datatools::configuration::item iCV2;
  datatools::configuration::variant_model & CV2 = iCV2.make_variant("medium_foil_thickness");
  CV2.set_documentation("Variant model associated to medium values of the source foil thickness");

  datatools::configuration::item iCV3;
  datatools::configuration::variant_model & CV3 = iCV3.make_variant("large_foil_thickness");
  CV3.set_documentation("Variant model associated to large values of the source foil thickness");

  // Create a parameter model:
  datatools::configuration::parameter_model CP;
  CP.set_name("source_foil_thickness");
  CP.set_documentation("This parameter enables the modification of the \n"
                       "average source foil thickness of the SuperNEMO\n"
                       "demonstrator module.");
  CP.set_display_name("Source foil thickness");
  CP.set_terse_description("The thickness of the source foil");

  CP.set_type(datatools::TYPE_REAL);
  CP.set_mutability(datatools::configuration::parameter_model::MUTABILITY_VARIABLE);
  CP.set_variable_mode(datatools::configuration::parameter_model::VARIABLE_MODE_INTERVAL);
  CP.set_real_unit_label("length");
  CP.set_real_preferred_unit("um");
  CP.set_real_precision(0.1 * CLHEP::micrometer);
  datatools::real_range rdomain(10.0 * CLHEP::micrometer, 100.0 * CLHEP::micrometer);
  rdomain.set_unit_label("length");
  CP.set_real_domain(rdomain);
  CP.set_default_real(15.0 * CLHEP::micrometer);

  // Attach variants to the parameter models:
  CP.add_variant("small",  iCV1.get_variant_handle(), "Variant for small foil thickness");
  CP.add_variant("medium", iCV2.get_variant_handle(), "Variant for medium foil thickness");
  CP.add_variant("large",  iCV3.get_variant_handle(), "Variant for large foil thickness");

  // Create 2 ranges:
  datatools::real_range small_thickness_domain(10.0 * CLHEP::micrometer, 20.0 * CLHEP::micrometer);
  datatools::real_range large_thickness_domain(50.0 * CLHEP::micrometer, 100.0 * CLHEP::micrometer,
                                               datatools::range_bound_excluded);
  CP.associate_variant_to_real_range(small_thickness_domain, "small");
  CP.associate_variant_to_real_range(large_thickness_domain, "large");
  // [10 um                             ,                      100 um]
  // [10 um , 20 um](20 um  ,    50 um](50 um       ,          100 um]
  CP.grab_auxiliaries().store_flag("linear_scale");
  CP.grab_auxiliaries().store("alpha", 6.66);
  CP.initialize_simple();
  CP.tree_dump(std::clog, "A configuration parameter : ", "--> ");
  CP.print_rst(std::cout, "");
  return;
}

void test1bis()
{
  std::clog << "\nTest 1 (bis):\n";

  datatools::configuration::item iCV1;
  datatools::configuration::variant_model & CV1 = iCV1.make_variant("far_negative_z_position");
  CV1.set_documentation("Variant model associated to large negative Z positions");

  datatools::configuration::item iCV2;
  datatools::configuration::variant_model & CV2 = iCV2.make_variant("near_zero_Z_position");
  CV2.set_documentation("Variant model associated to medium negative Z positions");

  datatools::configuration::item iCV3;
  datatools::configuration::variant_model & CV3 = iCV3.make_variant("far_positive_z_position");
  CV3.set_documentation("Variant model associated to large negative Z positions");

  datatools::configuration::parameter_model CP;
  CP.set_name("z_position");
  CP.set_documentation("This parameter enables the modification of the \n"
                       "position of some radioactive source along the Z axis.");
  CP.set_display_name("Source Z position");
  CP.set_terse_description("The Z position of the radioactive source");

  CP.set_type(datatools::TYPE_REAL);
  CP.set_real_unit_label("length");
  CP.set_real_preferred_unit("cm");
  CP.set_real_precision(0.1 * CLHEP::mm);
  CP.set_mutability(datatools::configuration::parameter_model::MUTABILITY_VARIABLE);
  CP.set_variable_mode(datatools::configuration::parameter_model::VARIABLE_MODE_FREE);
  CP.set_default_real(20.0 * CLHEP::cm);

  CP.add_variant("far_negative",  iCV1.get_variant_handle(), "Variant for large negative Z positions");
  CP.add_variant("near_zero",     iCV2.get_variant_handle(), "Variant for near zero Z positions");
  CP.add_variant("far_positive",  iCV3.get_variant_handle(), "Variant for large positive Z positions");

  datatools::real_range far_negative_region;
  far_negative_region.make_lower_unbounded(-50.0 * CLHEP::cm, datatools::range_bound_excluded);

  datatools::real_range near_zero_region;
  near_zero_region.make_bounded(-50.0 * CLHEP::cm, +50.0 * CLHEP::cm);

  datatools::real_range far_positive_region;
  far_positive_region.make_upper_unbounded(+50.0 * CLHEP::cm, datatools::range_bound_excluded);

  std::clog << "far_negative_region = " << far_negative_region << std::endl;
  std::clog << "near_zero_region    = " << near_zero_region    << std::endl;
  std::clog << "far_positive_region = " << far_positive_region << std::endl;

  CP.associate_variant_to_real_range(far_negative_region, "far_negative");
  CP.associate_variant_to_real_range(near_zero_region,    "near_zero");
  CP.associate_variant_to_real_range(far_positive_region, "far_positive");
  // (                             ,                                 )
  // (              ,-50 cm)[-50cm , +50cm](+50cm,                   )
  CP.initialize_simple();
  CP.tree_dump(std::clog, "A configuration parameter : ", "--> ");
  CP.print_rst(std::cout, "");
  return;
}

void test1ter()
{
  std::clog << "\nTest 1 (ter):\n";

  datatools::configuration::item iCV1;
  datatools::configuration::variant_model & CV1 = iCV1.make_variant("far_negative_z_position");
  CV1.set_documentation("Variant model associated to large negative Z positions");

  datatools::configuration::item iCV2;
  datatools::configuration::variant_model & CV2 = iCV2.make_variant("near_zero_Z_position");
  CV2.set_documentation("Variant model associated to medium negative Z positions");

  datatools::configuration::item iCV3;
  datatools::configuration::variant_model & CV3 = iCV3.make_variant("far_positive_z_position");
  CV3.set_documentation("Variant model associated to large negative Z positions");

  datatools::configuration::parameter_model CP;
  CP.set_name("z_position");
  CP.set_documentation("This parameter enables the modification of the \n"
                       "position of some radioactive source along the Z axis.");
  CP.set_display_name("Source Z position");
  CP.set_terse_description("The Z position of the radioactive source");

  CP.set_type(datatools::TYPE_REAL);
  CP.set_real_unit_label("length");
  CP.set_real_preferred_unit("cm");
  CP.set_real_precision(0.1 * CLHEP::mm);
  CP.set_mutability(datatools::configuration::parameter_model::MUTABILITY_VARIABLE);
  CP.set_variable_mode(datatools::configuration::parameter_model::VARIABLE_MODE_ENUM);
  CP.add_enumerated_value_real(-5 * CLHEP::m);
  CP.add_enumerated_value_real(-1 * CLHEP::m);
  CP.add_enumerated_value_real(-50 * CLHEP::cm);
  CP.add_enumerated_value_real(-20 * CLHEP::cm);
  CP.add_enumerated_value_real(-10 * CLHEP::cm);
  CP.add_enumerated_value_real(-5 * CLHEP::cm);
  CP.add_enumerated_value_real(-2 * CLHEP::cm);
  CP.add_enumerated_value_real(-1 * CLHEP::cm);
  CP.add_enumerated_value_real(+1 * CLHEP::cm);
  CP.add_enumerated_value_real(+2 * CLHEP::cm, true);
  CP.add_enumerated_value_real(+5 * CLHEP::cm);
  CP.add_enumerated_value_real(+10 * CLHEP::cm);
  CP.add_enumerated_value_real(+20 * CLHEP::cm);
  CP.add_enumerated_value_real(+50 * CLHEP::cm);
  CP.add_enumerated_value_real(+1 * CLHEP::m);
  CP.add_enumerated_value_real(+5 * CLHEP::m);

  CP.add_variant("far_negative",  iCV1.get_variant_handle(), "Variant for large negative Z positions");
  CP.add_variant("near_zero",     iCV2.get_variant_handle(), "Variant for near zero Z positions");
  CP.add_variant("far_positive",  iCV3.get_variant_handle(), "Variant for large positive Z positions");

  datatools::real_range far_negative_region;
  far_negative_region.make_lower_unbounded(-50.0 * CLHEP::cm, datatools::range_bound_excluded);

  datatools::real_range near_zero_region;
  near_zero_region.make_bounded(-50.0 * CLHEP::cm, +50.0 * CLHEP::cm);

  datatools::real_range far_positive_region;
  far_positive_region.make_upper_unbounded(+50.0 * CLHEP::cm, datatools::range_bound_excluded);

  std::clog << "far_negative_region = " << far_negative_region << std::endl;
  std::clog << "near_zero_region    = " << near_zero_region    << std::endl;
  std::clog << "far_positive_region = " << far_positive_region << std::endl;

  CP.associate_variant_to_real_range(far_negative_region, "far_negative");
  CP.associate_variant_to_real_range(near_zero_region,    "near_zero");
  CP.associate_variant_to_real_range(far_positive_region, "far_positive");
  // -5m -2m -1m -50cm -20cm -10cm -5cm -2cm -1cm +1cm +2cm +5cm +10cm +20cm +50cm +1m +2m +5m
  // (         ,-50 cm)[-50cm                    ,                        +50cm](+50cm,        )
  CP.initialize_simple();
  CP.tree_dump(std::clog, "A configuration parameter : ", "--> ");
  CP.print_rst(std::cout, "");

  double z = -10 * CLHEP::cm;
  if (CP.is_real_valid(z)) {
    std::clog << "z = " << z / CLHEP::cm << " cm is valid." << std::endl;
    std::string variant_name;
    if(CP.find_variant_associated_to_real(z, variant_name)) {
      std::clog << "z = " << z / CLHEP::cm << " cm is associated to variant '"
                << variant_name << "'." << std::endl;
      const datatools::configuration::variant_model & vm = CP.get_variant_model(variant_name);
      const std::string   & vd = CP.get_variant_description(variant_name);
      std::clog << "Variant '" << variant_name << "' has model '" << vm.get_name() << "'" << std::endl;
      std::clog << "  with description : '" << vd << "'" << std::endl;

    }
  }
  return;
}

void test2()
{
  std::clog << "\nTest 2:\n";

  datatools::configuration::parameter_model CP;
  CP.set_name("LHC_experiment");
  CP.set_documentation("This parameter can take different values \n"
                       "corresponding to the various physics experiments\n"
                       "in operation at LHC.");
  CP.set_display_name("The LHC experiment");
  CP.set_terse_description("The name of a physics experiment at LHC");
  CP.set(datatools::TYPE_STRING,
         datatools::configuration::parameter_model::VARIABLE_MODE_ENUM);
  CP.add_enumerated_value_string("Atlas", true);
  CP.add_enumerated_value_string("CMS");
  CP.add_enumerated_value_string("LHCb");
  CP.add_enumerated_value_string("Alice");
  CP.initialize_simple();
  CP.tree_dump(std::clog, "A configuration parameter : ", "");
  CP.print_rst(std::cout, "");
  return;
}

void test3()
{
  std::clog << "\nTest 3:\n";
  datatools::configuration::parameter_model CP;
  datatools::properties CP_setup;
  CP_setup.store_string("logging.priority", "debug");
  CP_setup.store_string("name", "geometry.detector_thickness");
  CP_setup.store_string("display_name", "GeoDetThickness");
  CP_setup.store_string("terse_description", "The thickness of the detector");
  CP_setup.store_string("type", "real");
  CP_setup.store_string("real.unit_label", "length");
  CP_setup.store_string("mutability", "variable");
  CP_setup.store_string("real.preferred_unit", "mm");
  CP_setup.store_real_with_explicit_unit("real.precision", 0.1 * CLHEP::mm);
  CP_setup.store_string("variable.mode", "interval");
  CP_setup.store_string("real.domain", "[10.0 cm, 50.0 cm]");
  CP_setup.store_real_with_explicit_unit("real.default", 20.0 * CLHEP::cm);
  CP_setup.store_string("aux.author", "John Doe");
  CP_setup.store_string("aux.date", "2014-05-14");
  CP_setup.store_string("aux.display_color", "blue");

  CP.initialize_standalone(CP_setup);

  CP.tree_dump(std::clog, "A configuration parameter : ", "");
  CP.print_rst(std::cout, "");
  return;
}

void test4()
{
  std::clog << "\nTest 4:\n";
  datatools::configuration::parameter_model CP;
  datatools::properties CP_setup;
  CP_setup.store_string("logging.priority", "debug");
  CP_setup.store_string("name", "geometry.detector_width");
  CP_setup.store_string("display_name", "GeoDetWidth");
  CP_setup.store_string("terse_description", "The width of the detector");
  CP_setup.store_string("type", "real");
  CP_setup.store_string("real.unit_label", "length");
  CP_setup.store_string("real.preferred_unit", "cm");
  CP_setup.store_real_with_explicit_unit("real.precision", 0.1 * CLHEP::mm);
  CP_setup.store_string("mutability", "variable");
  CP_setup.store_string("variable.mode", "enumeration");

  CP_setup.store("real.enumerated.size", 4);
  CP_setup.store_real_with_explicit_unit("real.enumerated_0.value", 10.0 * CLHEP::cm);
  CP_setup.store_real_with_explicit_unit("real.enumerated_1.value", 20.0 * CLHEP::cm);
  CP_setup.store_real_with_explicit_unit("real.enumerated_2.value", 50.0 * CLHEP::cm);
  CP_setup.store_real_with_explicit_unit("real.enumerated_3.value", 100.0 * CLHEP::cm);

  CP_setup.store_real_with_explicit_unit("real.default", 50.0 * CLHEP::cm);

  CP_setup.store("aux.author", "John Doe");
  CP_setup.store_real_with_explicit_unit("aux.alarm_threshold", 25.0 * CLHEP::cm);

  CP.initialize_standalone(CP_setup);
  CP.tree_dump(std::clog, "A configuration parameter : ", "");

  if (CP.is_real_valid(20.005 * CLHEP::cm)) {
    std::clog << "Value 20 cm is valid. \n";
  }

  if (! CP.is_real_valid(12.0 * CLHEP::cm)) {
    std::clog << "Value 12 cm is not valid. \n";
  }

  CP.print_rst(std::cout, "");

  return;
}

void test5()
{
  std::clog << "\nTest 5:\n";
  datatools::configuration::parameter_model CP;
  datatools::properties CP_setup;
  CP_setup.store_string("name", "geometry.magnetic_field");
  CP_setup.store_string("display_name", "GeoMagField");
  CP_setup.store_string("terse_description", "The magnetic field activitation");
  CP_setup.store_string("variable.mode", "enumeration");
  CP_setup.store_string("type", "boolean");
  CP_setup.store_boolean("boolean.default", false);

  CP.initialize_standalone(CP_setup);
  CP.tree_dump(std::clog, "A configuration parameter : ", "");

  CP.print_rst(std::cout, "");

  return;
}

int main (int /* argc_ */, char ** /* argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {

    test1();
    test1bis();
    test1ter();
    test2();
    test3();
    test4();
    test5();

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
