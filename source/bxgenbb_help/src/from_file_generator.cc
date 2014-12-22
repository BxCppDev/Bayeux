// from_file_generator.cc
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
#include <genbb_help/from_file_generator.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// This project:
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/save_to_file_wrapper.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(from_file_generator,"genbb::from_file_generator");

  struct from_file_generator::pimpl_imp
  {
    pimpl_imp();
    ~pimpl_imp();
    dpp::input_module inmod;  //!< Input module
    datatools::things record; //!< Working event record
  };

  from_file_generator::pimpl_imp::pimpl_imp()
  {
    return;
  }

  from_file_generator::pimpl_imp::~pimpl_imp()
  {
    return;
  }

  void from_file_generator::set_primary_event_bank_label(const std::string & label_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");
    _PE_label_ = label_;
    return;
  }

  from_file_generator::from_file_generator() : i_genbb()
  {
    _initialized_ = false;
    return;
  }

  from_file_generator::~from_file_generator()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  bool from_file_generator::is_initialized() const
  {
    return _initialized_;
  }

  void from_file_generator::reset()
  {
    DT_THROW_IF(!_initialized_, std::logic_error, "Operation prohibited ! Object is not initialized !");
    _initialized_ = false;
    if (_pimpl_) {
      _pimpl_->inmod.reset();
      _pimpl_.reset();
    }
    _PE_label_.clear();
    return;
  }

  void from_file_generator::initialize(const datatools::properties & config_,
                                       datatools::service_manager & /* service_manager_ */,
                                       detail::pg_dict_type & /* dictionary_ */)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");

    _initialize_base(config_);

    if (_PE_label_.empty()) {
      if (config_.has_key("PE.label")) {
        set_primary_event_bank_label(config_.fetch_string("PE.label"));
      }
    }
    if (_PE_label_.empty()) {
      set_primary_event_bank_label(save_to_file_wrapper::default_pe_label());
    }

    // Initialize the input module:
    _pimpl_.reset(new pimpl_imp);
    _pimpl_->inmod.initialize_standalone(config_);
    //_pimpl_->record.add<genbb::primary_event>(_PE_label_);

    _initialized_  = true;
    return;
  }

  bool from_file_generator::has_next()
  {
    if (_pimpl_ && !_pimpl_->inmod.is_terminated()) {
      return true;
    }
    return false;
  }

  void from_file_generator::_load_next(primary_event & event_,
                                       bool /* compute_classification_ */)
  {
    DT_LOG_TRACE(_logging_priority, "Entering...");
    event_.reset();
    _pimpl_->record.clear();
    if ( _pimpl_ && !_pimpl_->inmod.is_terminated()) {
      // Load the record with its embedded primary event bank:
      int status = _pimpl_->inmod.process(_pimpl_->record);
      if (status != dpp::base_module::PROCESS_OK) {
        DT_THROW_IF(true, std::logic_error,
                    "An error occured while loading the primary event !");
      }
      if (_pimpl_->record.is_a<genbb::primary_event>(_PE_label_)) {
        genbb::primary_event & PE = _pimpl_->record.grab<genbb::primary_event>(_PE_label_);
        event_ = PE;
      }
    }

    DT_LOG_TRACE(_logging_priority, "Exiting.");
    return;
  }

} // end of namespace genbb
