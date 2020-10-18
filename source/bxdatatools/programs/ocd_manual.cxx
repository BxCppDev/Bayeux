// ocd_manual.cxx

// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

// Third Party
// - Boost
#include <boost/program_options.hpp>

// - Bayeux:
#include <bayeux/bayeux.h>

// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/ocd_driver.h>
#include <datatools/kernel.h>

namespace datatools {

  // User interface for this app:
  struct ui {

    static
    void build_options(boost::program_options::options_description & opts_,
                       datatools::ocd_driver_params & params_);

    static
    void print_usage(boost::program_options::options_description & opts_,
                     std::ostream & out_);
  };

}

// Return kernel initialization flags for this application:
uint32_t app_kernel_init_flags();

/****************
 * Main program *
 ****************/
int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_, app_kernel_init_flags());

  int error_code = EXIT_SUCCESS;
  datatools::ocd_driver_params params;
  try {
    namespace po = boost::program_options;
    po::options_description opts ("Allowed options ");

    datatools::ui::build_options(opts, params);

    // Declare options for kernel:
    po::options_description optKernel("Kernel options");
    datatools::kernel::param_type paramsKernel;
    datatools::kernel::build_opt_desc(optKernel,
                                      paramsKernel,
                                      app_kernel_init_flags());

    // Aggregate options:
    po::options_description optPublic;
    optPublic.add(opts).add(optKernel);

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("class-id", 1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(optPublic)
      .positional(args)
      .allow_unregistered()
      .run();
    params.action_options =
      po::collect_unrecognized(parsed.options,
                               po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    // Fetch the opts/args :
    if (vm.count("help")) {
      if (vm["help"].as<bool>()) {
        datatools::ui::print_usage(optPublic, std::cout);
        return (1);
      }
    }

    if (vm.count("logging-priority")) {
      const std::string logging_label = vm["logging-priority"].as<std::string>();
      params.logging = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(params.logging == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    if (params.logging >= datatools::logger::PRIO_DEBUG) params.print(std::clog);

    datatools::ocd_driver ocdd;
    ocdd.initialize(params);
    ocdd.run();
    ocdd.reset();

  } catch (std::exception & x) {
    DT_LOG_FATAL(params.logging, x.what ());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(params.logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

namespace datatools {

  void ui::build_options(boost::program_options::options_description & opts_,
                         datatools::ocd_driver_params & params_)
  {
    namespace po = boost::program_options;
    opts_.add_options()

      ("help,h",
       po::value<bool>()
       ->zero_tokens()
       ->default_value(false),
       "Produce help message")

      ("logging-priority,P",
       po::value<std::string>()->default_value("fatal")
       ->value_name("level"),
       "Set the logging priority threshold.\n"
       "Example : \n"
       " --logging-priority \"debug\""
      )

      ("dlls-config,L",
       po::value<std::string>(&params_.dll_loader_config)
       ->value_name("file"),
       "Set the DLL loader configuration file.\n"
       "Example : \n"
       " --dlls-config \"dlls.conf\""
       )

      ("load-dll,l",
       po::value<std::vector<std::string> >(&params_.dlls)
       ->value_name("name[@path]"),
       "Set a DLL to be loaded.\n"
       "Example : \n"
       " --load-dll \"foo\" \n"
       " --load-dll \"foo@/opt/lib/foo/1.0\"  "
       )

      ("class-id,c",
       po::value<std::string>(&params_.class_id)
       ->value_name("classname"),
       "Set the ID/name of the class to be investigated. \n"
       "Example :\n"
       " --class-id \"genvtx::manager\""
       )

      ("action,a",
       po::value<std::string>(&params_.action)
       ->value_name("action"),
       "Define the action to be performed.\n"
       "Supported actions:\n"
       "* \"list\" : list the registered classes by ID. "
       "May use the '--load-dll LIBNAME' option.\n"
       "* \"show\" : print some description of the configuration parameters for class CLASSNAME. "
       "Needs the '--class-id CLASSNAME' option.\n"
       "* \"skeleton\" : print a sample skeleton configuration file for class CLASSNAME. "
       "Needs the '--class-id CLASSNAME' option. "
       "May use the '--output-file OUTPUT_FILE' option. \n"
       "* \"validate\" : validate an input configuration file for class CLASSNAME. "
       "Needs the '--class-id CLASSNAME' and '--input-file INPUTFILE' options."
       )

      ("input-file,i",
       po::value<std::string>(&params_.input_path)
       ->value_name("file"),
       "set the input file name."
       )

      ("output-file,o",
       po::value<std::string>(&params_.output_path)
       ->value_name("file"),
       "set the output file name."
       )

      ; // end of options' description
    return;
  }

  void ui::print_usage(boost::program_options::options_description & opts_,
                       std::ostream & out_)
  {
    const std::string APP_NAME = "bxocd_manual";
    out_ << "\n" << APP_NAME << " -- Object Configuration Description Manual" << std::endl;
    out_ << "                (Bayeux/datatools OCD support)" << std::endl;
    out_ << std::endl;
    out_ << "Usage : " << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " [OPTIONS] [ARGUMENTS] " << std::endl;
    out_ << opts_ << std::endl;
    out_ << "Examples : " << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " --help" << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " --action \"list\" " << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " --action \"list\" | grep ^datatools:: " << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " --load-dll \"foo\" --action \"list\"" << std::endl;
    out_  << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "    --class-id \"datatools::service_manager\" \\" << std::endl
         << "    --action \"show\" \\" << std::endl
         << "    [--no-configuration-infos] \\" << std::endl
         << "    [--no-title] \\" << std::endl
         << "    | pandoc -r rst -w plain \\" << std::endl
         << "    | less" << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "    --class-id \"datatools::service_manager\" \\" << std::endl
         << "    --action \"show\" \\" << std::endl
         << "    [--no-configuration-infos] \\" << std::endl
         << "    [--no-title] \\" << std::endl
         << "    | pandoc -T=\"datatools::service_manager\" \\" << std::endl
         << "             -r rst -w html \\" << std::endl
         << "             -o \"datatools_service_manager.html\" " << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "    --class-id \"datatools::service_manager\" \\" << std::endl
         << "    --action \"skeleton\" \\" << std::endl
         << "    [--no-additional-infos] \\" << std::endl
         << "    [--no-configuration-hints] \\" << std::endl
         << "    --output-file \"sm.conf\" " << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "    --class-id \"datatools::service_manager\" \\" << std::endl
         << "    --action \"validate\" \\" << std::endl
         << "    --input-file \"sm.conf\" " << std::endl;
    out_ << std::endl;
    return;
  }

}

uint32_t app_kernel_init_flags()
{
  uint32_t kernel_init_flags = 0;
  kernel_init_flags |= ::datatools::init_no_help;
  kernel_init_flags |= ::datatools::init_no_splash;
  kernel_init_flags |= ::datatools::init_no_inhibit_libinfo;
  kernel_init_flags |= ::datatools::init_no_libinfo_logging;
  kernel_init_flags |= ::datatools::init_no_variant;
  kernel_init_flags |= ::datatools::init_no_inhibit_variant;
  kernel_init_flags |= ::datatools::init_no_locale_category;
  kernel_init_flags |= ::datatools::init_no_inhibit_qt_gui;
  return kernel_init_flags;
}
