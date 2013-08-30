// -*- mode: c++ ; -*-
/* utils_module.cc
 */

#include <dpp/utils_module.h>

#include <cstdlib>
#include <stdexcept>
#include <sstream>

#include <datatools/types.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(utils_module,"dpp::utils_module");

  // Initialization :
  DPP_MODULE_INITIALIZE_IMPLEMENT_HEAD(utils_module,
                                       a_setup,
                                       a_service_manager,
                                       a_module_dict)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Utils module '" << get_name () << "' is already initialized ! ");

    _common_initialize(a_setup);

    if (a_setup.has_key ("mode")){
      _mode_ = MODE_INVALID;
      std::string mode_label = a_setup.fetch_string ("mode");
      if (mode_label == "clear") {
        _mode_ = MODE_CLEAR;
      } else if (mode_label == "remove_one_typed_bank") {
        _mode_ = MODE_REMOVE_ONE_TYPED_BANK;
      } else if (mode_label == "remove_banks") {
        _mode_ = MODE_REMOVE_BANKS;
      } else if (mode_label == "add_property") {
        _mode_ = MODE_ADD_PROPERTY;
      }
      DT_THROW_IF(_mode_ == MODE_INVALID,
                  std::domain_error,
                  "Module '" << get_name () << "' has no mode '" << mode_label << "' !");
    }

    if (_mode_ == MODE_REMOVE_ONE_TYPED_BANK) {
      DT_THROW_IF(! a_setup.has_key ("mode.remove_one_typed_bank.label"),
                  std::logic_error,
                  "Missing bank label property for module '" << get_name ()
                  << "' using mode '" << _mode_ << "' !");
      _remove_one_typed_bank_label_ = a_setup.fetch_string ("mode.remove_one_typed_bank.label");
      if (a_setup.has_key ("mode.remove_one_typed_bank.type")) {
        _remove_one_typed_bank_type_ = a_setup.fetch_string ("mode.remove_one_typed_bank.type");
      }
    }

    if (_mode_ == MODE_REMOVE_BANKS) {
      DT_THROW_IF(! a_setup.has_key ("mode.remove_banks.labels"),
                  std::logic_error,
                  "Missing bank label property for module '" << get_name ()
                  << "' using mode '" << _mode_ << "' !");
      a_setup.fetch ("mode.remove_banks.labels", _remove_bank_labels_);
      DT_THROW_IF(_remove_bank_labels_.size() == 0,
                  std::logic_error,
                  "Missing banks' labels for module '" << get_name ()
                  << "' using mode '" << _mode_ << "' !");
    }

    if (_mode_ == MODE_ADD_PROPERTY) {
      DT_THROW_IF(! a_setup.has_key ("mode.add_property.bank_label"),
                  std::logic_error,
                  "Missing 'mode.add_property.bank_label' property for module '" << get_name ()
                  << "' using mode '" << _mode_ << "' !");
      _add_property_bank_label_ = a_setup.fetch_string("mode.add_property.bank_label");
      DT_THROW_IF(! a_setup.has_key ("mode.add_property.key"),
                  std::logic_error,
                  "Missing 'mode.add_property.key' property for module '"
                  << get_name () << "' using mode '" << _mode_ << "' !");
      _add_property_key_ = a_setup.fetch_string("mode.add_property.key");
      if (a_setup.has_key ("mode.add_property.description")) {
        _add_property_description_ = a_setup.fetch_string("mode.add_property.description");
      }
      if (! _add_property_description_.empty()) {
        const datatools::properties::data & const_prop_data
          = a_setup.get("mode.add_property.value");
        datatools::properties::data & mutable_prop_data
          = const_cast<datatools::properties::data &>(const_prop_data);
        mutable_prop_data.set_description(_add_property_description_);
      }
      a_setup.export_and_rename_starting_with(_add_property_value_,
                                              "mode.add_property.value",
                                              _add_property_key_);
      if (a_setup.has_flag ("mode.add_property.update")) {
        _add_property_update_ = true;
      }
    }

    _set_initialized (true);
    return;
  }

  DPP_MODULE_RESET_IMPLEMENT_HEAD(utils_module)
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Utils module '" << get_name () << "' is not initialized !");
    _set_initialized (false);
    _set_defaults ();
    return;
  }

  void utils_module::_set_defaults ()
  {
    _mode_ = MODE_INVALID;
    _remove_one_typed_bank_label_ = "";
    _remove_one_typed_bank_type_ = "";
    _remove_bank_labels_.clear ();
    _add_property_bank_label_ = "";
    _add_property_key_ = "";
    _add_property_value_.clear();
    _add_property_update_ = false;
    return;
  }

  // Constructor :
  DPP_MODULE_CONSTRUCTOR_IMPLEMENT_HEAD(utils_module,logging_priority_)
  {
    _set_defaults ();
    return;
  }

  DPP_MODULE_DEFAULT_DESTRUCTOR_IMPLEMENT(utils_module)

  void utils_module::_process_clear (datatools::things & a_data_record)
  {
    a_data_record.clear ();
    return;
  }

  void utils_module::_process_remove_one_typed_bank (datatools::things & a_data_record)
  {
    const std::string & removable_bank_label = _remove_one_typed_bank_label_;
    DT_THROW_IF(removable_bank_label.empty (),
                std::logic_error,
                "Missing bank label to be removed !");
    bool remove = false;
    if (a_data_record.has (removable_bank_label)) {
      bool remove = true;
      if (! _remove_one_typed_bank_type_.empty()) {
        remove = false;
        if (a_data_record.has_serial_tag(removable_bank_label,
                                          _remove_one_typed_bank_type_)) {
          remove = true;
        }
      }
    }
    if (remove) {
      a_data_record.remove (removable_bank_label);
    }
    return;
  }

  void utils_module::_process_remove_banks (datatools::things & a_data_record)
  {
    for (size_t i = 0; i < _remove_bank_labels_.size(); i++) {
      const std::string & removable_bank_label = _remove_bank_labels_[i];
      DT_THROW_IF(removable_bank_label.empty (),
                  std::logic_error,
                  "Missing bank label to be removed !");
      if (a_data_record.has(removable_bank_label)) {
        a_data_record.remove(removable_bank_label);
      }
    }
    return;
  }

  void utils_module::_process_add_property (datatools::things & the_data_record)
  {
    DT_THROW_IF(_add_property_bank_label_.empty (),
                std::logic_error,
                "Missing property bank label to be enriched !");

    if (! the_data_record.has(_add_property_bank_label_)) {
      datatools::properties & the_gp_bank = the_data_record.add<datatools::properties> (_add_property_bank_label_);
      the_gp_bank.set_description("Bank of general properties");
    }

    if (the_data_record.has(_add_property_bank_label_)
        && the_data_record.is_a<datatools::properties> (_add_property_bank_label_)) {
      datatools::properties & the_gp_bank
        = the_data_record.grab<datatools::properties> (_add_property_bank_label_);
      if (the_gp_bank.has_key(_add_property_key_)) {
        DT_THROW_IF(! _add_property_update_,
                    std::logic_error,
                    "Property with key '" << _add_property_key_ << "' cannot be overloaded !");
        the_gp_bank.erase(_add_property_key_);
      }
      _add_property_value_.export_all(the_gp_bank);
    } else {
      DT_THROW_IF(true,
                  std::logic_error,
                  "No property bank label named '" << _add_property_bank_label_ << "' !");
    }
    return;
  }

  // Processing :
  DPP_MODULE_PROCESS_IMPLEMENT_HEAD(utils_module,a_data_record)
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Utils module '" << get_name () << "' is not initialized !");
    if (_mode_ == MODE_CLEAR){
      _process_clear (a_data_record);
    } else if (_mode_ == MODE_REMOVE_ONE_TYPED_BANK) {
      _process_remove_one_typed_bank (a_data_record);
    } else if (_mode_ == MODE_REMOVE_BANKS) {
      _process_remove_banks (a_data_record);
    } else if (_mode_ == MODE_ADD_PROPERTY) {
      _process_add_property (a_data_record);
    }
    return PROCESS_SUCCESS;
  }

} // end of namespace dpp

// end of utils_module.cc
