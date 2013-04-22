// -*- mode: c++ ; -*-
// ocd_manual.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <datatools/datatools_config.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/library_loader.h>

#include <datatools/detail/ocd_utils.h>

namespace datatools {

/// \brief Parameters of the OCD Manual application
struct ocd_manual_params
{
  /// Constructor
  ocd_manual_params();
  /// Reset
  void reset();
  /// Smart print
  void print(std::ostream & = std::cout) const;
public:
  bool debug; /// Debug flag
  bool interactive; /// Interactive flag
  std::vector<std::string> dlls; /// List of DLLs to be loaded
  std::string dll_loader_config; /// Configuration file of the DLL loader
  std::string action; /// The name of the action to invoke
  std::string class_id; /// The class registration ID
  std::vector<std::string> action_options; /// The options associated to the action
  std::string input_path; /// Input path
  std::string output_path; /// Output path
};

void ocd_manual_params::reset()
{
  debug = false;
  interactive = false;
  dlls.clear();
  dll_loader_config.clear();
  action.clear();
  action_options.clear();
  class_id.clear();
  input_path.clear();
  output_path.clear();
  return;
}

ocd_manual_params::ocd_manual_params()
{
  debug = false;
  interactive = false;
  return;
}

void ocd_manual_params::print(std::ostream & out_) const
{
  out_ << "OCD manual configuration parameters : " << '\n';
  out_ << "  Debug             = " << debug << '\n';
  out_ << "  Interactive       = " << interactive << '\n';
  out_ << "  DLL loader config = '" << dll_loader_config << "'" << '\n';
  out_ << "  DLLs = ";
  BOOST_FOREACH (const std::string & dll_name, this->dlls) {
    out_ << " '" << dll_name << "'";
  }
  out_ << std::endl;
  out_ << "  Action      = '" << action << "'" << '\n';
  out_ << "  Action options = ";
  BOOST_FOREACH (const std::string & action_opt, this->action_options) {
    out_ << " '" << action_opt << "'";
  }
  out_ << std::endl;
  out_ << "  Class ID    = '" << class_id << "'" << '\n';
  out_ << "  Input path  = '" << input_path << "'" << '\n';
  out_ << "  Output path = '" << output_path << "'" << '\n';
  return;
}

/// \brief The OCD manual application
class ocd_manual
{
public:
  /// Constructor
  ocd_manual();
  /// Check initialization status
  bool is_initialized() const;
  /// Initialization
  void initialize(const ocd_manual_params & params_);
  /// Run
  int run();
  /// Reset
  void reset();
protected:
  void _run_interactive();
  int _run_action();
  int _run_list();
  int _run_show(const std::string & class_id_);
  int _run_generate_skeleton(const std::string & class_id_,
                             const std::string & path_ = "");
  int _run_validate(const std::string & class_id_,
                    const std::string & path_);
private:
  bool _initialized_; /// Initialization flag
  ocd_manual_params _params_; /// Configuration parameters
};

ocd_manual::ocd_manual()
{
  _initialized_ = false;
  return;
}

bool ocd_manual::is_initialized() const
{
  return _initialized_;
}

void ocd_manual::initialize(const ocd_manual_params & params_)
{
  _params_ = params_;

  datatools::library_loader dll_loader(datatools::library_loader::allow_unregistered,
                                       _params_.dll_loader_config);
  BOOST_FOREACH (const std::string & dll_name, _params_.dlls) {
    if (_params_.debug) {
      std::clog << "DEBUG: " << "Loading DLL '" << dll_name << "'." << std::endl;
    }
    if (dll_loader.load (dll_name) != EXIT_SUCCESS) {
      std::ostringstream message;
      message << "ocd_manual: Loading DLL '" << dll_name << "' failed !";
      throw std::logic_error (message.str ());
    }
  }

  _initialized_ = true;
  return;
}

void ocd_manual::reset()
{
  _initialized_ = false;
  _params_.reset();
  return;
}

int ocd_manual::run()
{
  int error_code = EXIT_SUCCESS;
  if (! _initialized_) {
    throw std::logic_error("ocd_manual::run: Not initialized !");
  }

  if (_params_.interactive) {
    _run_interactive();
  }
  else {
    error_code = _run_action();
  }

  return error_code;
}

void ocd_manual::_run_interactive()
{
  std::cerr << datatools::io::warning
            << "datatools::ocd_manual::_run_interactive: Not implemented yet !"
            << std::endl;
  return;
}

int ocd_manual::_run_action()
{
  int error_code = EXIT_SUCCESS;

  if (_params_.action == "list") {
    error_code = _run_list();
  }
  else if (_params_.action == "show") {
    error_code = _run_show(_params_.class_id);
  }
  else if (_params_.action == "skeleton") {
    error_code = _run_generate_skeleton(_params_.class_id, _params_.output_path);
  }
  else if (_params_.action == "validate") {
    error_code = _run_validate(_params_.class_id, _params_.input_path);
  }
  else {
    std::cerr << datatools::io::error
              << "datatools::ocd_manual::_run_action: Unknown action '" << _params_.action  << "' !"
              << std::endl;
    error_code = EXIT_FAILURE;
  }

  return error_code;
}

int ocd_manual::_run_list()
{
  int error_code = EXIT_SUCCESS;
  const datatools::detail::ocd::ocd_registration & ocd_system_reg
    = datatools::detail::ocd::ocd_registration::get_system_registration();
  //ocd_system_reg.smart_dump(std::cerr, "The OCD system registation : ", "DEVEL: ");
  std::vector<std::string> ids;
  ocd_system_reg.compute_ids(ids);
  std::clog << "List of registered class IDs : " << '\n';
  BOOST_FOREACH (const std::string & id, ids) {
    std::cout << id << std::endl;
  }
  return error_code;
}

int ocd_manual::_run_show(const std::string & class_id_)
{
  int error_code = EXIT_SUCCESS;
  const datatools::detail::ocd::ocd_registration & ocd_system_reg
    = datatools::detail::ocd::ocd_registration::get_system_registration();
  //ocd_system_reg.smart_dump(std::cerr, "The OCD system registation : ", "DEVEL: ");

  if (class_id_.empty()) {
    std::cerr << datatools::io::error << "datatools::ocd_manual::_run_show: Missing class ID !"<< '\n';
    error_code = EXIT_FAILURE;
  }
  else if (! ocd_system_reg.has_id(class_id_)){
    std::cerr << datatools::io::error
              << "datatools::ocd_manual::_run_show: "
              << "Class ID '" << class_id_
              << "' is not registered in the datatools' OCD system register !"<< '\n';
    error_code = EXIT_FAILURE;
  }
  else {
    const datatools::object_configuration_description & OCD
      = ocd_system_reg.get(class_id_);
    uint32_t po_flags = 0;
    if (std::find(_params_.action_options.begin(),
                  _params_.action_options.end(),
                  "--no-configuration-infos") != _params_.action_options.end())
      {
        po_flags |= datatools::object_configuration_description::po_no_config;
      }
    OCD.print(std::cout, "", po_flags);
  }

  return error_code;
}

int ocd_manual::_run_generate_skeleton(const std::string & class_id_,
                                       const std::string & skeleton_path_)
{
  int error_code = EXIT_SUCCESS;

  if (class_id_.empty()) {
    std::cerr << datatools::io::error << "datatools::ocd_manual::_run_generate_skeleton: Missing class ID !"<< '\n';
    error_code = EXIT_FAILURE;
  }
  else if (! datatools::detail::ocd::ocd_registration::get_system_registration().has_id(class_id_)){
    std::cerr << datatools::io::error << "datatools::ocd_manual::_run_generate_skeleton: Class ID '" << class_id_ << "' is not registered in the datatools' OCD system register !"<< '\n';
    error_code = EXIT_FAILURE;
  }
  else {
    uint32_t sgo_flags = 0;
    if (std::find(_params_.action_options.begin(),
                  _params_.action_options.end(),
                  "--no-additional-infos") != _params_.action_options.end()) {
      sgo_flags |= datatools::object_configuration_description::sgo_no_add_infos;
    }
    if (std::find(_params_.action_options.begin(),
                  _params_.action_options.end(),
                  "--no-configuration-hints") != _params_.action_options.end()) {
      sgo_flags |= datatools::object_configuration_description::sgo_no_config_hints;
    }
    const datatools::object_configuration_description & OCD
      = datatools::detail::ocd::ocd_registration::get_system_registration().get(class_id_);
    if (skeleton_path_.empty()) {
      OCD.generate_sample_configuration(std::cout, "", sgo_flags);
    }
    else {
      std::string skeleton_path = skeleton_path_;
      datatools::fetch_path_with_env(skeleton_path);
      if (boost::filesystem::exists(skeleton_path)) {
        std::cerr << datatools::io::error << "datatools::ocd_manual::_run_generate_skeleton: File '" << skeleton_path << "' already exists ! Overwriting is not allowed !"<< '\n';
        error_code = EXIT_FAILURE;
      }
      else {
        std::ofstream fout(skeleton_path.c_str());
        if (!fout) {
          std::cerr << datatools::io::error
                    << "datatools::ocd_manual::_run_generate_skeleton: "
                    << "Cannot open file '" << skeleton_path << "' !"<< '\n';
          error_code = EXIT_FAILURE;
        }
        OCD.generate_sample_configuration(fout, "", sgo_flags);
      }
    }
  }

  return error_code;
}

int ocd_manual::_run_validate(const std::string & class_id_,
                              const std::string & setup_path_)
{
  int error_code = EXIT_SUCCESS;

  if (class_id_.empty()) {
    std::cerr << datatools::io::error << "datatools::ocd_manual::_run_validate: Missing class ID !"<< '\n';
    error_code = EXIT_FAILURE;
  }
  else if (! datatools::detail::ocd::ocd_registration::get_system_registration().has_id(class_id_)){
    std::cerr << datatools::io::error << "datatools::ocd_manual::_run_validate: Class ID '" << class_id_ << "' is not registered in the datatools' OCD system register !"<< '\n';
    error_code = EXIT_FAILURE;
  }
  else if (setup_path_.empty()) {
    std::cerr << datatools::io::error << "datatools::ocd_manual::_run_validate: Input filename is missing !"<< '\n';
    error_code = EXIT_FAILURE;
  }
  else {
    std::string setup_path = setup_path_;
    datatools::fetch_path_with_env(setup_path);
    if (! boost::filesystem::exists(setup_path)) {
      std::cerr << datatools::io::error << "datatools::ocd_manual::_run_validate: File '" << setup_path << "' does not exist !"<< '\n';
      error_code = EXIT_FAILURE;
    }
    else {
      datatools::properties setup;
      datatools::properties::read_config(setup_path, setup);
      const datatools::object_configuration_description & OCD
        = datatools::detail::ocd::ocd_registration::get_system_registration().get(class_id_);
      if (! OCD.has_validation_support()) {
        std::cerr << datatools::io::error << "datatools::ocd_manual::_run_validate: Class '" << class_id_ << "' has no validation support !"<< '\n';
        error_code = EXIT_FAILURE;
      }
      else {
        std::string error_message;
        if (! OCD.validate(setup, error_message)) {
          std::cerr << datatools::io::error
                    << "datatools::ocd_manual::_run_validate: "
                    << "Cannot validate setup file '" << setup_path << "' for class '"
                    << class_id_ << "' ! Reason is '" << error_message << "' !"<< '\n';
          error_code = EXIT_FAILURE;
        }
        else {
          std::clog << "File '" << setup_path << "' is validated for class '"
                    << class_id_ << "'." << '\n';
        }
      }
    }
  }

  return error_code;
}

} // namespace datatools

/****************
 * Main program *
 ****************/
int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::ocd_manual_params params;
  try {
    namespace po = boost::program_options;
    po::options_description opts ("Allowed options ");

    opts.add_options ()

      ("help,h", "produce help message")

      ("debug,d",
       po::value<bool>(&params.debug)
       ->zero_tokens()
       ->default_value (false),
       "produce debug logging")

      ("interactive,I",
       po::value<bool>(&params.interactive)
       ->zero_tokens()
       ->default_value (false),
       "run in interactive mode (not implemented)")

      ("dlls-config,L",
       po::value<std::string> (&params.dll_loader_config),
       "set the DLL loader configuration file.      \n"
       "Example :                                   \n"
       " --dlls-config dlls.conf                    "
       )

      ("load-dll,l",
       po::value<std::vector<std::string> >(&params.dlls),
       "set a DLL to be loaded.                     \n"
       "Example :                                   \n"
       " --load-dll genvtx                          "
       )

      ("class-id,c",
       po::value<std::string> (&params.class_id),
       "set the ID of the class to be investigated. \n"
       "Example :                                   \n"
       " --class-id genvtx::manager                 "
       )

      ("action,a",
       po::value<std::string> (&params.action),
       "define the action to be performed.                \n"
       "Actions:                                          \n"
       " list     : list the registered classes by ID     \n"
       "            May use '--load-dll DLL_NAME' option. \n"
       " show     : print some description                \n"
       "            of the configuration parameters       \n"
       "            for class CLASS_ID.                   \n"
       "            Needs '--class-id CLASS_ID' option.   \n"
       " skeleton : print a sample skeleton configuration \n"
       "            file for class CLASS_ID.              \n"
       "            Needs '--class-id CLASS_ID' option.   \n"
       "            May use '--output-file OUTPUT_FILE'   \n"
       "            option.                               \n"
       " validate : validate an input configuration       \n"
       "            file for class CLASS_ID.              \n"
       "            Needs '--class-id CLASS_ID' option.   \n"
       "            Needs '--input-file INPUT_FILE'       \n"
       "            option.                               "
       )

      ("input-file,i",
       po::value<std::string> (&params.input_path),
       "set the name of an input filename.")

      ("output-file,o",
       po::value<std::string> (&params.output_path),
       "set the name of an output filename.")

      ; // end of options' description


    // Describe command line arguments :
    po::positional_options_description args;
    args.add ("class-id", 1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser (argc_, argv_)
      .options (opts)
      .positional (args)
      .allow_unregistered()
      .run ();
    params.action_options = po::collect_unrecognized(parsed.options, po::include_positional);
    // po::store (po::command_line_parser (argc_, argv_)
    //            .options (opts)
    //            .positional (args)
    //            .allow_unregistered()
    //            .run (), vm);
    po::store (parsed, vm);
    po::notify (vm);

    // Fetch the opts/args :
    if (vm.count ("help")) {
      std::cout << "\nocd_manual -- Object Configuration Description Manual" << std::endl;
      std::cout << "                     (datatools OCD support)" << std::endl;
      std::cout << std::endl;
      std::cout << "Usage : " << std::endl;
      std::cout << std::endl;
      std::cout << "  ocd_manual [OPTIONS] [ARGUMENTS] " << std::endl << std::endl;
      std::cout << opts << std::endl;
      std::cout << "Examples : " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --help                                               " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --action list                                        " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --load-dll genvtx --action list                      " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --class-id datatools::service_manager \\             " << std::endl
                << "             --action show [--no-configuration-infos]             " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --class-id datatools::service_manager \\             " << std::endl
                << "             --action skeleton                     \\             " << std::endl
                << "             [--no-additional-infos]               \\             " << std::endl
                << "             [--no-configuration-hints]            \\             " << std::endl
                << "             --output-file sm.conf                                " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --class-id datatools::service_manager \\             " << std::endl
                << "             --action validate --input-file sm.conf               " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << std::endl;
      return (1);
    }

    if (params.debug) params.print(std::cerr);

    datatools::ocd_manual ocdm;
    ocdm.initialize(params);
    ocdm.run();
    ocdm.reset();

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of ocd_manual.cxx
