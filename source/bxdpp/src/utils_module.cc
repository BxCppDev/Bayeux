// -*- mode: c++ ; -*-
/* utils_module.cc
 */

#include <cstdlib>
#include <stdexcept>
#include <sstream>

#include <dpp/utils_module.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(utils_module,"dpp::utils_module");

  void utils_module::initialize (const DPP_DU::properties & a_setup,
                                 DPP_DS::service_manager & a_service_manager,
                                 module_handle_dict_type & a_module_dictionary)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "utils_module::initialize: "
                << "Module '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }

    if (! is_debug ())
      {
        if (a_setup.has_flag ("debug"))
          {
            set_debug (true);
          }
      }

    if (a_setup.has_key ("mode"))
      {
        std::string mode_label = a_setup.fetch_string ("mode");
        if (mode_label == "clear")
          {
            _mode_ = MODE_CLEAR;
          }
        else if (mode_label == "remove_one_typed_bank")
          {
            _mode_ = MODE_REMOVE_ONE_TYPED_BANK;
          }
        else if (mode_label == "remove_banks")
          {
            _mode_ = MODE_REMOVE_BANKS;
          }
        else
          {
            std::ostringstream message;
            message << "snemo::core::processing::utils_module::initialize: "
                    << "Module '" << get_name () << "' has no mode '" << mode_label << "' !";
            throw std::logic_error (message.str ());
          }
      }

    if (_mode_ == MODE_REMOVE_ONE_TYPED_BANK)
      {
        if (a_setup.has_key ("mode.remove_one_typed_bank.label"))
          {
            _remove_one_typed_bank_label_ = a_setup.fetch_string ("mode.remove_one_typed_bank.label");
          }
        else
          {
            std::ostringstream message;
            message << "dpp::utils_module::initialize: "
                    << "Missing bank label property for module '" << get_name () << "' using mode '" << _mode_ << "' !";
            throw std::logic_error (message.str ());
          }
        if (a_setup.has_key ("mode.remove_one_typed_bank.type"))
          {
            _remove_one_typed_bank_type_ = a_setup.fetch_string ("mode.remove_one_typed_bank.type");
          }
     }

    if (_mode_ == MODE_REMOVE_BANKS)
      {
        if (a_setup.has_key ("mode.remove_banks.labels"))
          {
            a_setup.fetch ("mode.remove_banks.labels", _remove_bank_labels_);
          }
        else
          {
            std::ostringstream message;
            message << "dpp::utils_module::initialize: "
                    << "Missing banks' labels property for module '" << get_name () << "' using mode '" << _mode_ << "' !";
            throw std::logic_error (message.str ());
          }
        if (_remove_bank_labels_.size() == 0)
          {
            std::ostringstream message;
            message << "dpp::utils_module::initialize: "
                    << "Missing banks' labels for module '" << get_name () << "' using mode '" << _mode_ << "' !";
            throw std::logic_error (message.str ());
          }
      }

    _set_initialized (true);
    return;
  }

  void utils_module::reset ()
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "utils_module::reset: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }

    _set_defaults ();

    _set_initialized (false);
    return;
  }

  void utils_module::_set_defaults ()
  {
    _mode_ = MODE_INVALID;

    _remove_one_typed_bank_label_ = "";
    _remove_one_typed_bank_type_ = "";
    _remove_bank_labels_.clear ();

    return;
  }

  // Constructor :
  utils_module::utils_module (int a_debug_level)
    : base_module ("dpp::utils_module",
                   "An event record processor to apply some useful operations",
                   "0.1",
                   a_debug_level)
  {
    _set_defaults ();
    return;
  }

  // Destructor :
  utils_module::~utils_module ()
  {
    // Make sure all internal resources are terminated
    // before destruction :
    if (is_initialized ())
      {
        this->utils_module::reset ();
      }
    return;
  }

  void utils_module::_process_clear (DPP_DU::things & a_event_record)
  {
    a_event_record.clear ();
    return;
  }

  void utils_module::_process_remove_one_typed_bank (DPP_DU::things & a_event_record)
  {
    const std::string & removable_bank_label = _remove_one_typed_bank_label_;
    if (removable_bank_label.empty ())
      {
        std::ostringstream message;
        message << "dpp::utils_module::_process_remove_bank: "
                << "Missing bank label to be removed !";
        throw std::logic_error (message.str ());
      }
    bool remove = false;
    if (a_event_record.has (removable_bank_label))
      {
        bool remove = true;
        if (! _remove_one_typed_bank_type_.empty())
          {
            remove = false;
            if (a_event_record.has_serial_tag(removable_bank_label,
                                              _remove_one_typed_bank_type_))
              {
                remove = true;
              }
          }
      }
    if (remove)
      {
        a_event_record.remove (removable_bank_label);
      }
    return;
  }

  void utils_module::_process_remove_banks (DPP_DU::things & a_event_record)
  {
    for (int i = 0; i < _remove_bank_labels_.size(); i++)
      {
        const std::string & removable_bank_label = _remove_bank_labels_[i];
        if (removable_bank_label.empty ())
          {
            std::ostringstream message;
            message << "dpp::utils_module::_process_remove_banks: "
                    << "Missing bank label to be removed !";
            throw std::logic_error (message.str ());
          }
        if (a_event_record.has (removable_bank_label))
          {
            a_event_record.remove (removable_bank_label);
          }
      }
    return;
  }
 
  // Processing :
  int utils_module::process (DPP_DU::things & a_event_record)
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "utils_module::process: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }

    /********************
     * Process the data *
     ********************/

    // Main processing method :
    if (_mode_ == MODE_CLEAR)
      {
        _process_clear (a_event_record);
      }
    else if (_mode_ == MODE_REMOVE_ONE_TYPED_BANK)
      {
        _process_remove_one_typed_bank (a_event_record);
      }
    else if (_mode_ == MODE_REMOVE_BANKS)
      {
        _process_remove_banks (a_event_record);
      }
 
    return SUCCESS;
  }

} // end of namespace dpp

// end of utils_module.cc
