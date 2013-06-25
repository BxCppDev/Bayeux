// -*- mode: c++ ; -*-
/* simulated_data_input_module.cc
 */

#include <mctools/simulated_data_input_module.h>

#include <datatools/io_factory.h>
#include <datatools/exception.h>

#include <mctools/simulated_data.h>
#include <mctools/simulated_data_reader.h>

namespace mctools {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(simulated_data_input_module, "mctools::simulated_data_input_module");

  const std::string simulated_data_input_module::DEFAULT_SD_BANK_LABEL = "SD";

  simulated_data_input_module::simulated_data_input_module(datatools::logger::priority logging_priority)
    : dpp::base_module(logging_priority)
  {
    _overwrite_existing_sd_bank_label_ = false;
    return;
  }


  // Destructor :
  simulated_data_input_module::~simulated_data_input_module ()
  {
    // Make sure all internal resources are terminated before destruction :
    if (is_initialized ()) reset ();
  }


  // Initialization :
  void simulated_data_input_module::initialize (const datatools::properties  & a_config,
                                                      datatools::service_manager   & a_service_manager,
                                                      dpp::module_handle_dict_type & a_module_dict)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is already initialized !");
    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/

    _common_initialize(a_config);

    // Initialize the simulated bank label:
    if (_sd_bank_label_.empty()) {
      if (a_config.has_key("SD_bank.label")) {
        _sd_bank_label_ = a_config.fetch_string("SD_bank.label");
      }
    }

    if (a_config.has_flag("SD_bank.overwrite_existing")) {
      _overwrite_existing_sd_bank_label_ = true;
    }

    // Initialize the reader :
    _reader_.reset(new mctools::simulated_data_reader);
    datatools::properties reader_config;
    a_config.export_and_rename_starting_with(reader_config, "input.", "");
    reader_config.tree_dump(std::clog, "Reader configuration parameters: ");
    _reader_.get()->initialize(reader_config);

    // Finalize initialization :
    if (_sd_bank_label_.empty()) {
      _sd_bank_label_ = DEFAULT_SD_BANK_LABEL;
    }

    // Tag the module as initialized :
    _set_initialized (true);
    return;
  }

  bool simulated_data_input_module::is_terminated () const
  {
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is not initialized !");
    return _reader_.get()->is_terminated();
  }

  // Reset :
  void simulated_data_input_module::reset ()
  {
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is not initialized !");

    // Tag the module as un-initialized :
    _set_initialized (false);

    /****************************
     *  revert to some defaults *
     ****************************/
    if (_reader_) {
      _reader_.get()->reset ();
      _reader_.reset(0);
    }
    _sd_bank_label_.clear();
    _overwrite_existing_sd_bank_label_ = false;

    /****************************
     *  end of the reset step   *
     ****************************/

    return;
  }


  // Processing :
  int simulated_data_input_module::process (datatools::things & a_record)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Simulated data reader module '" << get_name () << "' is not initialized !");
    return _load (a_record);
  }


  int simulated_data_input_module::_load (datatools::things & a_record)
  {
    int load_status = dpp::PROCESS_OK;

    simulated_data * SD = 0;
    if (a_record.has(_sd_bank_label_)) {
      DT_THROW_IF (! a_record.is_a<simulated_data>(_sd_bank_label_),
                   std::logic_error,
                   "Existing data bank named '" << _sd_bank_label_ << "' does not contain simulated data !");
      DT_THROW_IF (! _overwrite_existing_sd_bank_label_,
                   std::logic_error,
                   "Existing simulated data bank named '" << _sd_bank_label_ << "' cannot be overwritten !");
      simulated_data & the_SD = a_record.grab<simulated_data>(_sd_bank_label_);
      the_SD.reset();
      SD = &the_SD;
    } else {
      simulated_data & the_SD = a_record.add<simulated_data>(_sd_bank_label_);
      SD = &the_SD;
    }

    if (_reader_.get()->has_next()) {
      int ret = _reader_.get()->load_next(*SD);
      if (ret != 0) {
        load_status = dpp::PROCESS_ERROR;
      }
    } else {
      load_status = dpp::PROCESS_FATAL;
    }

    return load_status;
  }

} // end of namespace mctools
// OCD support for class 'mctools::simulated_data_input_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::simulated_data_input_module,ocd_)
{
  ocd_.set_class_name ("mctools::simulated_data_input_module");
  ocd_.set_class_library ("mctools");
  ocd_.set_class_description ("A module to load Monte-Carlo plain events from a set data files");



  ocd_.set_configuration_hints ("A ``mctools::simulated_data_input_module`` object can be setup with the    \n"
                                "following syntax in a ``datatools::multi_properties`` configuration  \n"
                                "file, typically from a module manager object.                        \n"
                                "                                                                     \n"
                                "Example::                                                            \n"
                                "                                                                     \n"
                                "  #@description A module that read simulated data from mctools::g4 simulation output\n"
                                "  #@key_label   \"name\"                                             \n"
                                "  #@meta_label  \"type\"                                             \n"
                                "                                                                     \n"
                                "  [name=\"sd_reader\" type=\"mctools::g4::simulated_data_input_module\"]\n"
                                "  #@config A simulated data reader module                            \n"
                                "                                                                     \n"
                                "  #@description The logging priority                                 \n"
                                "  logging.priority : string = \"fatal\"                              \n"
                                "                                                                     \n"
                                "  #@description The simulated data bank label                        \n"
                                "  SD_bank.label : string = \"SD\"                                    \n"
                                "                                                                     \n"
                                "  #@description Allow simulated data bank overwriting                \n"
                                "  SD_bank.overwrite_existing : boolean = 0                           \n"
                                "                                                                     \n"
                                "  #@description Input max number of files                            \n"
                                "  input.max_files : integer =  10                                    \n"
                                "                                                                     \n"
                                "  #@description Input max number of records                          \n"
                                "  input.max_record_total : integer =  100000                         \n"
                                "                                                                     \n"
                                "  #@description Input max number of records per file                 \n"
                                "  input.max_record_per_file : integer =  10000                       \n"
                                "                                                                     \n"
                                "  #@description Input file mode                                      \n"
                                "  input.files.mode : string = \"single\"                             \n"
                                "                                                                     \n"
                                "  #@description Input file mode                                      \n"
                                "  input.single.filename : string = \"sd.xml\"                        \n"
                                "                                                                     \n"
                                "or::                                                                 \n"
                                "                                                                     \n"
                                "  #@description Input file mode                                      \n"
                                "  input.files.mode : string = \"list\"                               \n"
                                "                                                                     \n"
                                "  #@description List of input files                                  \n"
                                "  input.files.list.filenames : string[2] = \"sd0.xml\" \"sd1.xml\"   \n"
                                "                                                                     \n"
                                );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::simulated_data_input_module,"mctools::simulated_data_input_module")

// end of simulated_data_input_module.cc
