/// \file datatools/test_configuration_variant_api_0.cxx

// Ourselves:
#include <datatools/configuration/variant_registry_manager.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <fstream>

// This project:
#include <datatools/datatools_config.h>
#include <datatools/datatools.h>
#include <datatools/kernel.h>
#include <datatools/properties.h>

// - Bayeux:
#include <bayeux/bayeux.h>

#if DATATOOLS_WITH_QT_GUI == 1

// Third party:
// - Qt:
#include <QApplication>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QStyleFactory>

#include <datatools/configuration/ui/variant_registry_tree_model.h>
#include <datatools/configuration/ui/variant_registry_dialog.h>
#include <datatools/configuration/ui/variant_repository_viewer.h>
#include <datatools/configuration/ui/variant_repository_dialog.h>

#endif // DATATOOLS_WITH_QT_GUI == 1

void test0(bool debug_ = false);

void test1(bool debug_ = false, bool gui_ = false);

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {

    bool debug = false;
    bool do_test0 = false;
    bool do_test1 = false;
    bool do_gui = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-0" || arg == "--test0") do_test0 = true;
      if (arg == "-1" || arg == "--test1") do_test1 = true;
#if DATATOOLS_WITH_QT_GUI == 1
      if (arg == "--gui") do_gui = true;
#endif
      iarg++;
    }

    if (do_test0) test0(debug);

    if (do_test1) test1(debug, do_gui);

  }
  catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...){
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}

void test0(bool debug_)
{
  std::clog << "\nTest 0:\n";
  datatools::configuration::variant_registry_manager mgr;

  datatools::properties mgr_config;
  std::string mgr_config_filename =
    "${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_registry_manager.conf";
  datatools::fetch_path_with_env(mgr_config_filename);
  datatools::properties::read_config(mgr_config_filename, mgr_config);
  mgr.initialize(mgr_config);
  if (debug_) mgr.tree_dump(std::clog, "Variant manager:");

  datatools::configuration::variant_registry vreg;
  vreg.initialize_from(mgr,                    /* The factory manager */
                       "if_radioactive.VM",    /* The variant model */
                       "source_radioactivity", /* The name of the registry */
                       "Source radioactivity", /* The display name of the registry */
                       "Configuration of the radioactive source" /* The description of the registry */
                       );
  if (debug_) vreg.tree_dump(std::clog, "Variant registry: ");

  datatools::configuration::variant_registry vreg2;
  vreg2.initialize_from(mgr,          /* The factory manager */
                        "geometry.VM" /* The variant model */
                        );
  if (debug_) vreg2.tree_dump(std::clog, "Variant registry 2: ");

  {
    datatools::command::returned_info ri =
      vreg2.cmd_set_parameter_value("has_detector_0", "1");
    ri =
      vreg2.cmd_set_parameter_value("has_detector_1", "1");
    ri =
      vreg2.cmd_set_parameter_value("has_detector_1/if_detector/thickness",
                                    "0.00029 m");
  }
  datatools::configuration::ascii_io aio;
  std::clog << std::endl << "Dump of the first registry:" << std::endl;
  aio.store_registry(std::clog, vreg);
  std::clog << std::endl << "Dump of the second registry:" << std::endl;
  aio.store_registry(std::clog, vreg2);
  std::clog << std::endl;
  return;
}

void test1(bool debug_, bool
#if DATATOOLS_WITH_QT_GUI == 1
           gui_
#endif // DATATOOLS_WITH_QT_GUI == 1
)
{
  std::clog << "\nTest 1:\n";

  datatools::configuration::variant_registry_manager mgr;
  mgr.set_logging_priority(datatools::logger::PRIO_FATAL);
  // mgr.set_name("geometry");
  // mgr.set_display_name("Geometry");
  // mgr.set_terse_description("Configuration variants for the geometry");
  mgr.set_top_variant_name("geometry.VM");
  mgr.add_property_prefix("visu");
  mgr.add_property_prefix("alarm");
  std::string base_def_file     = "${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_base.defs";
  std::string geometry_def_file = "${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_geometry.defs";
  mgr.load(base_def_file);
  mgr.load(geometry_def_file);
  mgr.initialize_simple();
  if (debug_) mgr.tree_dump(std::clog, mgr.get_display_name());

  datatools::configuration::variant_registry geometry_vreg;
  geometry_vreg.set_name("geometry");
  geometry_vreg.set_display_name("Geometry");
  geometry_vreg.set_terse_description("Configuration variants for the geometry");
  geometry_vreg.initialize_from(mgr);
  if (debug_) geometry_vreg.tree_dump(std::clog, "Variant registry: ");

  datatools::configuration::variant_registry radioactivity_vreg;
  radioactivity_vreg.set_name("radioactivity");
  radioactivity_vreg.set_display_name("Radioactivity");
  radioactivity_vreg.set_terse_description("Configuration variants for the setup radioactivity");
  radioactivity_vreg.initialize_from(mgr, "if_radioactive.VM");
  if (debug_) radioactivity_vreg.tree_dump(std::clog, "Variant registry: ");

  datatools::configuration::variant_registry vreg;
  vreg.set_name("geometry"); // The same name !
  vreg.set_display_name("Geometry");
  vreg.set_terse_description("Configuration variants for the geometry");
  vreg.initialize_from(mgr, "if_radioactive.VM");
  if (debug_) vreg.tree_dump(std::clog, "Variant registry: ");

  datatools::configuration::variant_repository vrep;
  vrep.set_name("my_experiment");
  vrep.set_display_name("My experiment");
  vrep.set_terse_description("The repository for my experiment configuration parameters");
  vrep.registration_external(radioactivity_vreg,
                             "radioactivity"
                             );
  vrep.registration_external(geometry_vreg,
                             "geometry"
                             );
  vrep.registration_embedded("${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_registry_manager.conf",
                             "geometry.VM",
                             "geometry2" /* Choose this name to avoid registry name clash in the repository */
                             );
  vrep.registration_embedded("${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_registry_manager_2.conf",
                             "vertex_generation.VM",
                             "vertex_generation", /* Choose a registration name */
                             "Vertex generation", /* Choose a registration display name */
                             "Configuration for the vertex generation" /* Terse description */
                             );

  if (debug_) vrep.tree_dump(std::clog, "The variant repository: ");

  datatools::configuration::variant_registry & active_vreg =
    vrep.grab_registry("geometry");

  datatools::configuration::variant_registry & active_vreg2 =
    vrep.grab_registry("radioactivity");

  {
    datatools::command::returned_info ri
      = active_vreg.cmd_set_parameter_value("logging", "trace");
    if (ri.is_success()) {
      std::clog << "Command was successful!" << std::endl;
      if (debug_) active_vreg.tree_dump(std::clog, "Variant registry: ");
    } else {
      std::cerr << "Command failed!" << std::endl;
      std::cerr << ri.get_error_message() << std::endl;
    }
  }

  {
    datatools::command::returned_info ri
      = active_vreg.cmd_set_parameter_value("has_detector_1/if_detector/length","1 mm");
    if (ri.is_success()) {
      std::clog << "Command was successful!" << std::endl;
      if (debug_) active_vreg.tree_dump(std::clog, "Variant registry: ");
    } else {
      std::cerr << "Command failed!" << std::endl;
      std::cerr << ri.get_error_message() << std::endl;
    }
  }

  {
    datatools::command::returned_info ri
      = active_vreg.cmd_set_parameter_value("has_detector_1","1");
    if (ri.is_success()) {
      std::clog << "Command was successful!" << std::endl;
      if (debug_) active_vreg.tree_dump(std::clog, "Variant registry: ");
    } else {
      std::cerr << "Command failed!" << std::endl;
      std::cerr << ri.get_error_message() << std::endl;
    }
  }

  {
    datatools::command::returned_info ri
      = active_vreg.cmd_set_parameter_value("has_detector_1/if_detector/length","1 mm");
    if (ri.is_success()) {
      std::clog << "Command was successful!" << std::endl;
      if (debug_) active_vreg.tree_dump(std::clog, "Variant registry: ");
    } else {
      std::cerr << "Command failed!" << std::endl;
      std::cerr << ri.get_error_message() << std::endl;
    }
  }

  {
    datatools::command::returned_info ri
      = active_vreg.cmd_set_parameter_value("has_detector_1/if_detector/length","4 cm");
    if (ri.is_success()) {
      std::clog << "Command was successful!" << std::endl;
      if (debug_) active_vreg.tree_dump(std::clog, "Variant registry: ");
    } else {
      std::cerr << "Command failed!" << std::endl;
      std::cerr << ri.get_error_message() << std::endl;
    }
  }

  {
    datatools::command::returned_info ri
      = active_vreg.cmd_set_parameter_value("has_detector_1/if_detector/material", "Germanium");
    if (ri.is_success()) {
      std::clog << "Command was successful!" << std::endl;
      if (debug_) active_vreg.tree_dump(std::clog, "Variant registry: ");
    } else {
      std::cerr << "Command failed!" << std::endl;
      std::cerr << ri.get_error_message() << std::endl;
    }
  }

  std::clog << std::endl;
  std::clog << "---------------------------------------------------------------" << std::endl;
  std::clog << "Registry: " << std::endl;
  std::clog << "---------------------------------------------------------------" << std::endl;
  uint32_t io_flags =
    datatools::configuration::ascii_io::IO_DESCRIPTION
    | datatools::configuration::ascii_io::IO_TITLE;
  if (debug_) {
    io_flags |= datatools::configuration::ascii_io::IO_TRACE;
  }
  datatools::configuration::ascii_io aw(io_flags);
  aw.store_registry(std::clog, active_vreg);
  std::clog << "---------------------------------------------------------------" << std::endl;
  std::clog << std::endl;

  if (! datatools::kernel::is_instantiated()) {
    std::clog << "datatools' kernel is not instantiated." << std::endl;
  } else {
    std::clog << "datatools' kernel is instantiated." << std::endl;

    datatools::kernel & dtkl = datatools::kernel::instance();
    if (dtkl.has_variant_repository()) {
      datatools::configuration::variant_repository & dkvr = dtkl.grab_variant_repository();
      dkvr.registration_external(active_vreg);
      dkvr.registration_external(active_vreg2);

      if (debug_) dkvr.tree_dump(std::clog, "Datatools' kernel variant repository: ");
      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << "Kernel variant repository: "<< std::endl;
      std::clog << "---------------------------------------------------------------" << std::endl;
      aw.store_repository(std::clog, dkvr);
      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << std::endl;

      // Modify the value of a variant parameter:
      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_1/if_detector/length",
                                 "5.2 cm");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      // Modify the value of a variant parameter:
      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_1/if_detector/material",
                                 "Silicium");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      // Modify the value of a variant parameter:
      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_1",
                                 "0");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      // Modify the value of a variant parameter:
      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_0",
                                 "1");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      // Modify the value of a variant parameter:
      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_0/if_detector/material",
                                 "Betty");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      // Modify the value of a variant parameter:
      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "firstname",
                                 "Betty");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << "Kernel variant repository (2): "<< std::endl;
      std::clog << "---------------------------------------------------------------" << std::endl;
      aw.store_repository(std::clog, dkvr);
      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << std::endl;

      // Modify the value of a variant parameter:
      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_0/if_detector/material",
                                 "Germanium");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << "Kernel variant repository (3): "<< std::endl;
      std::clog << "---------------------------------------------------------------" << std::endl;
      aw.store_repository(std::clog, dkvr);
      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << std::endl;

      std::clog << std::endl;
      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << "Repository: " << std::endl;
      std::clog << "---------------------------------------------------------------" << std::endl;
      aw.store_repository(std::clog, dkvr);
      std::clog << "---------------------------------------------------------------" << std::endl;
      std::clog << std::endl;

      {
        std::ofstream vo("variants.rep");
        aw.store_repository(vo, dkvr);
        vo.close();
        std::clog << std::endl;
      }

      {
        std::ifstream vi("variants.rep");
        int error = aw.load_repository(vi, dkvr);
        if (error == 0) {
          vi.close();
          aw.store_repository(std::clog, dkvr);
          std::clog << std::endl;
        } else {
          std::cerr << "DEVEL: Failed to load the repository!" << std::endl;
        }
      }

      // {
      //   std::clog << "DEVEL: From file..." << std::endl;
      //   std::string variants_filename = "${DATATOOLS_TESTING_DIR}/config/experimental_setup.rep";
      //   datatools::fetch_path_with_env(variants_filename);
      //   std::ifstream vi(variants_filename.c_str());
      //   if (!vi) {
      //     std::clog << "DEVEL: Cannot open file '" << variants_filename << "'" << std::endl;
      //   } else {
      //     std::clog << "DEVEL: Open file '" << variants_filename << "'" << std::endl;
      //     int error = aw.load_repository(vi, dkvr);
      //     if (error == 0) {
      //       vi.close();
      //       std::clog << "DEVEL:  Print..." << std::endl;
      //       aw.store_repository(std::clog, dkvr);
      //       std::clog << std::endl;
      //     } else {
      //       std::cerr << "DEVEL: Failed to load the repository from file '"
      //                 << variants_filename << "'!" << std::endl;
      //     }
      //   }
      // }

      std::clog << std::endl << std::endl;
      // Load a new registry (math):
      dkvr.registration_embedded("${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_registry_manager_3.conf");
      if (debug_) dkvr.tree_dump(std::clog, "Datatools' kernel variant repository (with 'math'): ");
      std::clog << std::endl << std::endl;
      dkvr.lock();

      {
        // Parse a property file:
        datatools::properties config;
        std::string config_filename = "${DATATOOLS_TESTING_DIR}/config/test_properties_sample-variant0.conf";
        datatools::fetch_path_with_env(config_filename);
        datatools::properties::read_config(config_filename, config);
        config.tree_dump(std::clog, "Configuration properties: ");
      }

#if DATATOOLS_WITH_QT_GUI == 1
      std::cerr << "DEVEL: DATATOOLS_WITH_QT_GUI..." << std::endl;
      if (gui_) {
        int argc = 1;
        const char * argv[] = { "Bayeux - Test Configuration variant" };
        // QApplication::setStyle(QStyleFactory::create("cleanlooks"));
        QApplication::setStyle(QStyleFactory::create("plastique"));
        QApplication app(argc, (char**) argv);

        {
          datatools::configuration::ui::variant_registry_tree_model vrtm;
          vrtm.construct(dkvr.grab_registry("geometry"), "Geometry setup");
          vrtm.tree_dump(std::clog, "Settings tree model:");

          {
            datatools::configuration::ui::variant_registry_dialog window(vrtm);
            window.show();
            int ret = app.exec();
            if (ret == QDialog::Rejected) {
              DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Variant registry dialog was rejected!");
            }
          }
          dkvr.get_registry("geometry").tree_dump(std::clog, "Settings:");
        }

        {
          QWidget * window = new QWidget;
          QVBoxLayout * layout = new QVBoxLayout;
          datatools::configuration::ui::variant_repository_viewer * vr_viewer
            = new datatools::configuration::ui::variant_repository_viewer;
          vr_viewer->set_repository(dkvr);
          layout->addWidget(vr_viewer);
          window->setLayout(layout);
          window->show();
          int ret = app.exec();
          if (ret == QDialog::Rejected) {
            DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Variant repository viewer was rejected!");
          }
        }

        {
          datatools::configuration::ui::variant_repository_dialog window(dkvr);
          window.show();
          int ret = app.exec();
          if (ret == QDialog::Rejected) {
            DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Variant repository dialog was rejected!");
          }
        }

      }
      std::cerr << "DEVEL: End of DATATOOLS_WITH_QT_GUI." << std::endl;
#endif // DATATOOLS_WITH_QT_GUI == 1

      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_0",
                                 "true");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      {
        datatools::command::returned_info ri =
          dkvr.cmd_set_parameter("geometry",
                                 "has_detector_0/if_detector/length",
                                 "2 cm");
        if (ri.is_failure()) {
          std::cerr << "ERROR: Command failed: " << ri.get_error_message() << std::endl;
        }
      }

      {
        // Parse a property file:
        datatools::properties config;
        std::string config_filename = "${DATATOOLS_TESTING_DIR}/config/test_properties_sample-variant1.conf";
        datatools::fetch_path_with_env(config_filename);
        datatools::properties::read_config(config_filename, config);
        config.tree_dump(std::clog, "Configuration properties: ");
      }

    }
  }

  return;
}
