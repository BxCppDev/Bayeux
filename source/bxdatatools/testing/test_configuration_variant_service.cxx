/// \file datatools/test_configuration_variant_service.cxx

// Ourselves:
#include <datatools/configuration/variant_service.h>

// Standard library:
#include <iostream>
#include <string>
#include <stdexcept>

// This project:
#include <datatools/properties.h>
#include <datatools/properties_config.h>

/// Parameters for the application
struct app_config
{
  datatools::configuration::variant_service::config variant;
};

/// The processing
struct processing
{
  processing();
  void initialize(const datatools::properties & config_);
  void run();
  void reset();
};

int main(int argc_, char ** argv_)
{
  bayeux::initialize();
  int error_code = EXIT_SUCCESS;
  app_config params;

  namespace bpo = boost::program_options;
  namespace dtc = datatools::configuration;

  // Declare options:
  bpo::options_description optDesc("Options");
  optDesc.add_options()
    ("help,h",
     "print this help message")
    // ("value",
    //  bpo::value<int>(&params.value)
    //  ->default_value(42)
    //  ->value_name("[value]"),
    //  "a value")
    ;

  // Declare options for variant support:
  bpo::options_description optVariant("Variant support");
  uint32_t po_init_flags = 0;
  // po_init_flags |= dtc::variant_service::NO_CONFIG_FILENAME;
  po_init_flags |= dtc::variant_service::PROFILE_LOAD_DONT_IGNORE_UNKNOWN;
  dtc::variant_service::init_options(optVariant,
                                     params.variant,
                                     po_init_flags);

  // Aggregate options:
  bpo::options_description optPublic;
  optPublic.add(optDesc).add(optVariant);

  // Parse options:
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::parse_command_line(argc_, argv_, optPublic), vMap);
    bpo::notify(vMap);
  } catch (const bpo::required_option& e) {
    // We need to handle help even if required_option thrown
    if (!vMap.count("help")) {
      std::cerr << "[OptionsException] " << e.what() << std::endl;
      throw std::logic_error(e.what());
    }
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    throw std::logic_error(e.what());
  }

  // Handle any non-bound options:
  if (vMap.count("help")) {
    std::cout << "Usage:\n"
              << " test_configuration_variant_service [options]\n"
              << optPublic
              << "\n";
  } else {

    // Variant support:
    try {
      if (params.variant.config_filename.empty()) {
        params.variant.config_filename = "${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_repository.conf";
      }
      params.variant.print(std::clog, "Variant parameters: ");
      dtc::variant_service app_var_serv;
      if (params.variant.is_active()) {
        app_var_serv.configure(params.variant);
        app_var_serv.start();
      }

      // Application scope:
      {
        datatools::properties pconfig;
        {
          std::string config_name = "${DATATOOLS_TESTING_DIR}/config/test_configuration_variant_service.conf";
          uint32_t reader_opts = 0;
          reader_opts |= datatools::properties_config::RESOLVE_PATH;
          datatools::properties_config reader(reader_opts);
          reader.read(config_name,pconfig);
          pconfig.tree_dump(std::clog, "Configuration parameters after variant processing: ");
        }

        processing p;
        p.initialize(pconfig);
        p.run();
        p.reset();
      }

      // Terminate variant service:
      if (app_var_serv.is_started()) {
        app_var_serv.stop();
      }

    } catch (std::exception& e) {
      std::cerr << "[error] " << e.what()
                << std::endl;
      error_code = EXIT_FAILURE;
      throw std::logic_error(e.what());
    }

    // std::cerr << "DEVEL: Terminate variant service..." << std::endl;
    // std::cerr << "DEVEL: Done." << std::endl;

  }
  bayeux::terminate();
  return error_code;
}

processing::processing()
{
  std::clog << "Constructing processing...\n";
  return;
}

void processing::initialize(const datatools::properties & /*config_*/)
{
  std::clog << "Initializing processing .\n";
  return;
}

void processing::run()
{
  std::clog << "Running processing...\n";
  return;
}

void processing::reset()
{
  std::clog << "Reseting processing...\n";
  return;
}
