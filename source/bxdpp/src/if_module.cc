/* if_module.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
#include <datatools/things.h>
#include <datatools/ioutils.h>
// - Bayeux/cuts:
#include <cuts/cut_service.h>
#include <cuts/cut_manager.h>
#include <cuts/i_cut.h>

// Ourselves:
#include <dpp/if_module.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(if_module, "dpp::if_module");

  const std::string & if_module::default_cut_service_label()
  {
    static std::string label = "Cuts";
    return label;
  }

  void if_module::set_cut_service_label(const std::string & label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is initialized !");
    _cut_service_label_ = label_;
    return;
  }

  const std::string & if_module::get_cut_service_label() const
  {
    return _cut_service_label_;
  }

  void if_module::set_condition_cut(const std::string & label_,
                                    const cuts::cut_handle_type & condition_cut_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is initialized !");
    _condition_cut_.label  = label_;
    _condition_cut_.handle = condition_cut_;
    return;
  }

  bool if_module::has_then_status() const
  {
    return ! (_then_status_ & PROCESS_INVALID);
  }

  bool if_module::has_else_status() const
  {
    return ! (_else_status_ & PROCESS_INVALID);
  }

  void if_module::set_then_status(process_status status_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is initialized !");
    const process_status status = status_;
    DT_THROW_IF(status != PROCESS_INVALID
                && status != PROCESS_CONTINUE
                && status != PROCESS_STOP,
                std::logic_error,
                "If module '" << get_name() << "' has invalid *then status* " << status_ << " !");
    _then_status_ = status_;
    if (! has_else_status() && ! _else_module_.handle) {
      _else_status_ = PROCESS_STOP;
    }
    return;
  }

  void if_module::set_else_status(process_status status_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is initialized !");
    const process_status status = status_;
    DT_THROW_IF(status != PROCESS_INVALID
                && status != PROCESS_CONTINUE
                && status != PROCESS_STOP,
                std::logic_error,
                "If module '" << get_name() << "' has invalid *else status* " << status_ << " !");
    _else_status_ = status_;
    if (! has_then_status() && ! _then_module_.handle) {
      _then_status_ = PROCESS_STOP;
    }
    return;
  }

  bool if_module::has_then_module() const
  {
    return _then_module_.handle;
  }

  bool if_module::has_else_module() const
  {
    return _else_module_.handle;
  }

  void if_module::set_then_module(const module_entry & then_module_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is initialized !");
    _then_module_ = then_module_;
    if (has_then_status()) {
      _then_status_ = PROCESS_INVALID;
    }
    return;
  }

  void if_module::set_else_module(const module_entry & else_module_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is initialized !");
    _else_module_ = else_module_;
    if (has_else_status()) {
      _else_status_ = PROCESS_INVALID;
    }
    return;
  }

  void if_module::_set_defaults()
  {
    _cut_service_label_ = "";
    _then_status_ = PROCESS_INVALID;
    _else_status_ = PROCESS_INVALID;
    return;
  }

  /*** Implementation of the interface ***/

  // Constructor :
  if_module::if_module(datatools::logger::priority logging_priority_)
    : base_module(logging_priority_)
  {
    _set_defaults();
    return;
  }

  if_module::~if_module()
  {
    if (is_initialized()) if_module::reset();
    return;
  }

  // Initialization :
  void if_module::initialize(const datatools::properties & a_config,
                             datatools::service_manager & a_service_manager,
                             dpp::module_handle_dict_type & a_module_dict)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is already initialized ! ");
    _common_initialize(a_config);

    if (_cut_service_label_.empty()) {
      if (a_config.has_key("cut_service.label")) {
        std::string label = a_config.fetch_string("cut_service.label");
        set_cut_service_label(label);
      }
    }

    if (! _condition_cut_.handle) {
      if (_cut_service_label_.empty()) {
        set_cut_service_label(default_cut_service_label());
      }

      bool check = a_service_manager.has(_cut_service_label_)
        && a_service_manager.is_a<cuts::cut_service>(_cut_service_label_);
      DT_THROW_IF(! check,
                  std::logic_error,
                  "If module '" << get_name() << "' can't find any service with label '" << _cut_service_label_
                  << "' from the service manager ! ");
      cuts::cut_service & the_cut_service
        = a_service_manager.grab<cuts::cut_service>(_cut_service_label_);
      cuts::cut_manager & the_cut_manager = the_cut_service.grab_cut_manager ();
      DT_THROW_IF(! a_config.has_key("condition_cut"),
                  std::logic_error,
                  "Missing 'condition_cut' property ! ");
      std::string cut_name = a_config.fetch_string("condition_cut");
      cuts::cut_handle_dict_type::iterator found
        = the_cut_manager.get_cuts().find(cut_name);
      DT_THROW_IF(found == the_cut_manager.get_cuts().end(),
                  std::logic_error,
                  "If module '" << get_name() << "' can't find any cut named '" << cut_name
                  << "' from the cut manager ! ");
      _condition_cut_.label  = found->first;
      _condition_cut_.handle = found->second.grab_initialized_cut_handle();
    }

    // 2012 FM: add support for enforced 'then/else status' (without associated modules) :
    if (! has_then_status()) {
      if (a_config.has_key("then_status"))  {
        std::string then_status = a_config.fetch_string("then_status");
        if (then_status == "continue") {
          set_then_status(PROCESS_CONTINUE);
        } else if (then_status == "stop") {
          set_then_status(PROCESS_STOP);
        }
        DT_THROW_IF(! has_then_status(),
                    std::logic_error,
                    "If module '" << get_name() << "' has invalid value '"
                    << then_status << "' for the 'then_status' property ! ");
      }
    }

    if (! has_else_status()) {
      if (a_config.has_key("else_status")) {
        std::string else_status = a_config.fetch_string("else_status");
        if (else_status == "continue") {
          set_else_status(PROCESS_CONTINUE);
        } else if (else_status == "stop") {
          set_else_status(PROCESS_STOP);
        }
        DT_THROW_IF(! has_else_status(),
                    std::logic_error,
                    "If module '" << get_name() << "' has invalid value '"
                    << else_status << "' for the 'else_status' property ! ");
      }
    }

    // If no then_status is defined, try to associate a processing module :
    if (! has_then_status()) {
      if (! _then_module_.handle) {
        DT_THROW_IF(! a_config.has_key("then_module"),
                    std::logic_error,
                    "Missing 'then_module' property !");
        std::string module_name = a_config.fetch_string("then_module");
        module_handle_dict_type::iterator found
          = a_module_dict.find(module_name);

        DT_THROW_IF(found == a_module_dict.end(),
                    std::logic_error,
                    "If module '" << get_name() << "' can't find any module named '" << module_name
                    << "' from the external dictionnary ! ");
        _then_module_.label = found->first;
        _then_module_.handle = found->second.grab_initialized_module_handle();
      }
    }

    // If no else_status is defined, try to associate a processing module :
    if (! has_else_status()) {
      if (! _else_module_.handle) {
        DT_THROW_IF(! a_config.has_key("else_module"),
                    std::logic_error,
                    "Missing 'else_module' property !");
        std::string module_name = a_config.fetch_string("else_module");
        module_handle_dict_type::iterator found
          = a_module_dict.find(module_name);

        DT_THROW_IF(found == a_module_dict.end(),
                    std::logic_error,
                    "If module '" << get_name() << "' can't find any module named '" << module_name
                    << "' from the external dictionnary ! ");
        _else_module_.label = found->first;
        _else_module_.handle = found->second.grab_initialized_module_handle();
      }
    }

    _set_initialized(true);
    return;
  }

  // Reset :
  void if_module::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    // Reset handles :
    _condition_cut_.handle.reset();
    _then_module_.handle.reset();
    _else_module_.handle.reset();
    // Restore default values :
    _set_defaults();
    return;
  }

  // Processing :
  base_module::process_status
  if_module::process(::datatools::things & the_data_record)
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "If module '" << get_name() << "' is not initialized !");
    this->reset_last_error_message();
    process_status status = PROCESS_ERROR;
    cuts::i_cut & the_cut = _condition_cut_.handle.grab();
    // Prepare the user data to be checked by the active 'cut':
    the_cut.set_user_data<datatools::things>(the_data_record);
    // Apply the cut algorithm:
    DT_LOG_DEBUG(_logging, "Checking cut '" << _condition_cut_.label << "'...");
    int cut_status = the_cut.process();
    // Detach user data.
    the_cut.reset_user_data();
    // Check the cut's returned value:
    if (cut_status == cuts::SELECTION_INAPPLICABLE) {
      DT_LOG_DEBUG(_logging, "Cut '" << _condition_cut_.label << "' was inapplicable.");
      status = PROCESS_ERROR_STOP;
    } else if (cut_status == cuts::SELECTION_ACCEPTED) {
      DT_LOG_DEBUG(_logging, "Cut '" << _condition_cut_.label << "' has been checked.");
      status = PROCESS_SUCCESS;
      const process_status then_status = _then_status_;
      if (then_status != PROCESS_INVALID) {
        if (then_status == PROCESS_CONTINUE) {
          status = PROCESS_SUCCESS;
        } else {
          status = PROCESS_STOP;
        }
      } else {
        base_module & the_then_module = _then_module_.handle.grab();
        DT_LOG_DEBUG(_logging, "Applying module '" << _then_module_.label << "'...");
        the_then_module.reset_last_error_message();
        status = the_then_module.process(the_data_record);
      }
      return status;
    } else {
      DT_LOG_DEBUG(_logging, "Cut '" << _condition_cut_.label << "' has NOT been checked.");
      status = PROCESS_SUCCESS;
      const process_status else_status = _else_status_;
      if (else_status != PROCESS_INVALID) {
        if (else_status == PROCESS_CONTINUE) {
          status = PROCESS_SUCCESS;
        } else {
          status = PROCESS_STOP;
        }
      } else {
        if (_else_module_.handle) {
          base_module & the_else_module = _else_module_.handle.grab();
          DT_LOG_DEBUG(_logging, "Applying module '" << _else_module_.label << "'...");
          the_else_module.reset_last_error_message();
          status = the_else_module.process(the_data_record);
        }
      }
    }
    return status;
  }

  void if_module::tree_dump(std::ostream      & out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool inherit_) const
  {
    this->base_module::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Cut service label : '" << _cut_service_label_ << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Condition cut     : '" << _condition_cut_.label << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Then status       : '" << _then_status_ << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Then module       : '" << _then_module_.label << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Else status       : '" << _else_status_ << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Else module       : '" << _else_module_.label << "'" <<std:: endl;

    return;
  }

}  // end of namespace dpp

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
