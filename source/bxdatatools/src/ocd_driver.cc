// ocd_driver.cc

// Ourselves:
#include <datatools/ocd_driver.h>

// Standard Library:
#include <sstream>
#include <fstream>
#include <stdexcept>

// Third Party:
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/library_loader.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/detail/ocd_utils.h>

namespace datatools {

void ocd_driver_params::reset()
{
  logging = datatools::logger::PRIO_NOTICE;
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

ocd_driver_params::ocd_driver_params()
{
  interactive = false;
  return;
}

void ocd_driver_params::print(std::ostream & out_) const
{
  out_ << "OCD manual configuration parameters : " << std::endl;
  out_ << "  Interactive       = " << interactive << std::endl;
  out_ << "  Logging priority  = "
       << datatools::logger::get_priority_label(logging) << std::endl;
  out_ << "  DLL loader config = '" << dll_loader_config << "'" << std::endl;
  out_ << "  DLLs = ";
  BOOST_FOREACH (const std::string & dll_name, this->dlls) {
    out_ << " '" << dll_name << "'";
  }
  out_ << std::endl;
  out_ << "  Action      = '" << action << "'" << std::endl;
  out_ << "  Action options = ";
  BOOST_FOREACH (const std::string & action_opt, this->action_options) {
    out_ << " '" << action_opt << "'";
  }
  out_ << std::endl;
  out_ << "  Class ID    = '" << class_id << "'" << std::endl;
  out_ << "  Input path  = '" << input_path << "'" << std::endl;
  out_ << "  Output path = '" << output_path << "'" << std::endl;
  return;
}

ocd_driver::ocd_driver()
{
  _initialized_ = false;
  return;
}

ocd_driver::~ocd_driver()
{
  if (is_initialized()) {
    reset();
  }
  return;
}

bool ocd_driver::is_initialized() const
{
  return _initialized_;
}

void ocd_driver::initialize(const ocd_driver_params & params_)
{
  _params_ = params_;
  _dll_loader_.reset(new datatools::library_loader(_params_.dll_loader_config));
  BOOST_FOREACH (const std::string & dll_name, _params_.dlls) {
    DT_LOG_DEBUG(_params_.logging, "Loading DLL '" << dll_name << "'...");
    DT_THROW_IF (_dll_loader_->load (dll_name) != EXIT_SUCCESS,
                 std::logic_error,
                 "Loading DLL '" << dll_name << "' failed !");
  }

  _initialized_ = true;
  return;
}

void ocd_driver::reset()
{
  _initialized_ = false;
  _dll_loader_.reset();
  _params_.reset();
  return;
}

int ocd_driver::run()
{
  int error_code = EXIT_SUCCESS;
  DT_THROW_IF (! _initialized_, std::logic_error, "Not initialized !");

  if (_params_.interactive) {
    _run_interactive();
  } else {
    error_code = _run_action();
  }

  return error_code;
}

void ocd_driver::_run_interactive()
{
  DT_LOG_WARNING(_params_.logging, "Not implemented yet !");
  return;
}

int ocd_driver::_run_action()
{
  int error_code = EXIT_SUCCESS;

  if (_params_.action == "list") {
    error_code = _run_list();
  } else if (_params_.action == "show") {
    error_code = _run_show(_params_.class_id);
  } else if (_params_.action == "skeleton") {
    error_code = _run_generate_skeleton(_params_.class_id, _params_.output_path);
  } else if (_params_.action == "validate") {
    error_code = _run_validate(_params_.class_id, _params_.input_path);
  } else {
    DT_LOG_ERROR(_params_.logging,
                 "Unknown action '" << _params_.action  << "' !");
    error_code = EXIT_FAILURE;
  }

  return error_code;
}

int ocd_driver::_run_list(std::ostream & out_, std::ostream & log_)
{
  int error_code = EXIT_SUCCESS;
  const datatools::detail::ocd::ocd_registration & ocd_system_reg
    = datatools::detail::ocd::ocd_registration::get_system_registration();
  //ocd_system_reg.smart_dump(std::cerr, "The OCD system registation : ", "DEVEL: ");
  std::vector<std::string> ids;
  ocd_system_reg.compute_ids(ids);
  log_ << "List of registered class IDs : " << std::endl;
  BOOST_FOREACH (const std::string & id, ids) {
    out_ << id << std::endl;
  }
  return error_code;
}

int ocd_driver::_run_show(const std::string & class_id_, std::ostream & out_)
{
  int error_code = EXIT_SUCCESS;
  const datatools::detail::ocd::ocd_registration & ocd_system_reg
    = datatools::detail::ocd::ocd_registration::get_system_registration();
  //ocd_system_reg.smart_dump(std::cerr, "The OCD system registation : ", "DEVEL: ");

  if (class_id_.empty()) {
    DT_LOG_ERROR(_params_.logging, "Missing class ID !");
    error_code = EXIT_FAILURE;
  } else if (! ocd_system_reg.has_id(class_id_)) {
    DT_LOG_ERROR(_params_.logging, "Class ID '" << class_id_
                 << "' is not registered in the datatools' OCD system register !");
    error_code = EXIT_FAILURE;
  } else {
    const datatools::object_configuration_description & OCD
      = ocd_system_reg.get(class_id_);
    uint32_t po_flags = 0;
    if (std::find(_params_.action_options.begin(),
                  _params_.action_options.end(),
                  "--no-configuration-infos") != _params_.action_options.end()) {
        po_flags |= datatools::object_configuration_description::po_no_config;
      }
    if (std::find(_params_.action_options.begin(),
                  _params_.action_options.end(),
                  "--no-title") != _params_.action_options.end()) {
        po_flags |= datatools::object_configuration_description::po_no_title;
      }
    OCD.print(out_, "", po_flags);
  }

  return error_code;
}

int ocd_driver::_run_generate_skeleton(const std::string & class_id_,
                                       const std::string & skeleton_path_)
{
  int error_code = EXIT_SUCCESS;
  if (class_id_.empty()) {
    DT_LOG_ERROR(_params_.logging, "Missing class ID !");
    error_code = EXIT_FAILURE;
  } else if (! datatools::detail::ocd::ocd_registration::get_system_registration().has_id(class_id_)){
    DT_LOG_ERROR(_params_.logging, "Class ID '" << class_id_
                 << "' is not registered in the datatools' OCD system register !");
    error_code = EXIT_FAILURE;
  } else {
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
    } else {
      std::string skeleton_path = skeleton_path_;
      datatools::fetch_path_with_env(skeleton_path);
      if (boost::filesystem::exists(skeleton_path)) {
        DT_LOG_ERROR(_params_.logging, "File '" << skeleton_path << "' already exists ! "
                     << "Overwriting is not allowed !");
        error_code = EXIT_FAILURE;
      } else {
        std::ofstream fout(skeleton_path.c_str());
        if (!fout) {
          DT_LOG_ERROR(_params_.logging, "Cannot open file '" << skeleton_path << "' !");
          error_code = EXIT_FAILURE;
        }
        OCD.generate_sample_configuration(fout, "", sgo_flags);
      }
    }
  }

  return error_code;
}

int ocd_driver::_run_validate(const std::string & class_id_,
                              const std::string & setup_path_)
{
  int error_code = EXIT_SUCCESS;
  if (class_id_.empty()) {
    DT_LOG_ERROR(_params_.logging, "Missing class ID !");
    error_code = EXIT_FAILURE;
  }else if (! datatools::detail::ocd::ocd_registration::get_system_registration().has_id(class_id_)){
    DT_LOG_ERROR(_params_.logging, "Class ID '" << class_id_
                 << "' is not registered in the datatools' OCD system register !");
    error_code = EXIT_FAILURE;
  } else if (setup_path_.empty()) {
    DT_LOG_ERROR(_params_.logging, "Input filename is missing !");
    error_code = EXIT_FAILURE;
  } else {
    std::string setup_path = setup_path_;
    datatools::fetch_path_with_env(setup_path);
    if (! boost::filesystem::exists(setup_path)) {
      DT_LOG_ERROR(_params_.logging, "File '" << setup_path << "' does not exist !");
      error_code = EXIT_FAILURE;
    } else {
      datatools::properties setup;
      datatools::read_config(setup_path, setup);
      const datatools::object_configuration_description & OCD
        = datatools::detail::ocd::ocd_registration::get_system_registration().get(class_id_);
      if (! OCD.has_validation_support()) {
        DT_LOG_ERROR(_params_.logging,"Class '" << class_id_ << "' has no validation support !");
        error_code = EXIT_FAILURE;
      } else {
        std::string error_message;
        if (! OCD.validate(setup, error_message)) {
          DT_LOG_ERROR(_params_.logging,
                       "Cannot validate setup file '" << setup_path << "' for class '"
                       << class_id_ << "' ! Reason is '" << error_message << "' !");
          error_code = EXIT_FAILURE;
        } else {
          DT_LOG_NOTICE(_params_.logging, "File '" << setup_path << "' is validated for class '"
                        << class_id_ << "'.");
        }
      }
    }
  }

  return error_code;
}

} // end of namespace datatools
