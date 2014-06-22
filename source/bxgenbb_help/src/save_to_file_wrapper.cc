// save_to_file_wrapper.cc
/*
 * Copyright 2014 F. Mauger
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

// Ourselves:
#include <genbb_help/save_to_file_wrapper.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
// - Bayeux/dpp:
#include <dpp/output_module.h>

// This project:
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(save_to_file_wrapper,"genbb::save_to_file_wrapper");

  struct save_to_file_wrapper::pimpl_imp
  {
    pimpl_imp();
    ~pimpl_imp();
    dpp::output_module outmod; /// Output module
    datatools::things record; /// Working event record
  };

  // static
  const std::string & save_to_file_wrapper::default_pe_label()
  {
    static std::string _label("genbb.PE");
    return _label;
  }

  save_to_file_wrapper::pimpl_imp::pimpl_imp()
  {
    return;
  }

  save_to_file_wrapper::pimpl_imp::~pimpl_imp()
  {
    return;
  }

  save_to_file_wrapper::save_to_file_wrapper() : i_genbb()
  {
    _initialized_ = false;
    _pg_ = 0;
    return;
  }

  save_to_file_wrapper::~save_to_file_wrapper()
  {
    if (_initialized_) {
      reset ();
    }
    return;
  }

  void save_to_file_wrapper::set_primary_event_bank_label(const std::string & label_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");
    _PE_label_ = label_;
    return;
  }

  void save_to_file_wrapper::set_generator(i_genbb & pg_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");
    DT_THROW_IF(! pg_.is_initialized(), std::logic_error, "Generator '" << pg_.get_name() << "' is not initialized !");
    _pg_ = &pg_;
    return;
  }

  bool save_to_file_wrapper::is_initialized() const
  {
    return _initialized_;
  }

  void save_to_file_wrapper::reset()
  {
    DT_THROW_IF(!_initialized_, std::logic_error, "Operation prohibited ! Object is not initialized !");
    if (_pimpl_) {
      _pimpl_->outmod.reset();
      _pimpl_.reset();
    }
    _PE_label_.clear();
    _pg_ = 0;
    _initialized_ = false;
    return;
  }

  void save_to_file_wrapper::initialize(const datatools::properties & config_,
                                         datatools::service_manager & service_manager_,
                                         detail::pg_dict_type & dictionary_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");

    _initialize_base(config_);

    if (_pg_ == 0) {
      DT_THROW_IF (!config_.has_key("generator"),
                   std::logic_error,
                   "Missing 'generator' key !");

      // Fetch particle generator name:
      const std::string & pg_name = config_.fetch_string("generator");

      // Find it from the dictionary:
      detail::pg_dict_type::iterator found = dictionary_.find(pg_name);
      DT_THROW_IF (found == dictionary_.end(), std::logic_error,
                   "No particle generator named '" << pg_name << "' found !");
      detail::pg_entry_type & pet = found->second;
      if (! pet.is_initialized()) {
        datatools::factory_register<i_genbb> * facreg = 0;
        // Check if a manager can provide a factory object :
        mygsl::rng * xprng = 0;
        if (has_external_random()) {
          xprng = &grab_external_random();
        }
        if (pet.has_manager()) {
          facreg = &pet.grab_manager().grab_factory_register();
          if (xprng == 0) {
            if (pet.get_manager().has_external_prng()) {
              xprng = &pet.grab_manager().grab_external_prng();
            }
          }
        }
        detail::initialize(pet,
                           &service_manager_,
                           &dictionary_,
                           facreg,
                           xprng);
      }
      set_generator(pet.grab());
    }

    if (_PE_label_.empty()) {
      if (config_.has_key("PE.label")) {
        set_primary_event_bank_label(config_.fetch_string("PE.label"));
      }
    }
    if (_PE_label_.empty()) {
      set_primary_event_bank_label(default_pe_label());
    }

    // Initialize the ouput module:
    _pimpl_.reset(new pimpl_imp);
    _pimpl_->outmod.initialize_standalone(config_);
    _pimpl_->record.add<genbb::primary_event>(_PE_label_);

    _initialized_  = true;
    return;
  }

  bool save_to_file_wrapper::has_next()
  {
    DT_THROW_IF(_pg_ == 0, std::logic_error,
                "Null particle generator !");
    return _pg_->has_next();
  }

  void save_to_file_wrapper::_load_next(primary_event & event_,
                                         bool compute_classification_)
  {
    DT_LOG_TRACE(_logging_priority, "Entering...");
    DT_THROW_IF(_pg_ == 0, std::logic_error,
                "Null particle generator !");
    _pg_->load_next(event_, compute_classification_);

    if ( _pimpl_ && !_pimpl_->outmod.is_terminated()) {
      genbb::primary_event & PE = _pimpl_->record.grab<genbb::primary_event>(_PE_label_);
      PE = event_;
      // Save the record with its embedded primary event bank:
      int status = _pimpl_->outmod.process(_pimpl_->record);
      if (status != dpp::base_module::PROCESS_OK) {
        DT_THROW_IF(true, std::logic_error,
                    "An error occured while saving the primary event !");
      }
    }

    DT_LOG_TRACE(_logging_priority, "Exiting.");
    return;
  }

} // end of namespace genbb
