// nuclear_decay.cc
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
#include <genbb_help/nuclear_decay.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/clhep_units.h>
// - Bayeux/materials:
#include <materials/isotope.h>

// This project:
#include <genbb_help/nuclear_level.h>
#include <genbb_help/beta_decay.h>
#include <genbb_help/gamma_transition.h>

namespace genbb {


  nuclear_decay::nuclear_decay()
  {
    _initialized_ = false;
    _set_default();
    return;
  }

  nuclear_decay::~nuclear_decay()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void nuclear_decay::initialize_simple()
  {
    datatools::properties dummy;
    initialize(dummy, 0);
    return;
  }

  bool nuclear_decay::is_initialized() const
  {
    return _initialized_;
  }

  void nuclear_decay::initialize(const datatools::properties & config_,
                                 const std::map<std::string, datatools::handle<nuclear_level> > * levels_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Decay is already initialized !");

    // config_.tree_dump(std::cerr, "NC config_:", "DEVEL: ");

    // Fetch parameters:
    if (levels_) {
      if (_level_initial_ == 0) {
        if (config_.has_key("level_initial")) {
          const std::string & level_initial_label = config_.fetch_string("level_initial");
          std::map<std::string, nuclear_level::handle_type>::const_iterator level_found = levels_->find(level_initial_label);
          DT_THROW_IF(level_found == levels_->end(), std::logic_error,
                      "Cannot find initial level '" << level_initial_label << "' from dictionary of levels !");
          set_level_initial(level_found->second.get());
        }
      }

      if (_level_final_ == 0) {
        if (config_.has_key("level_final")) {
          const std::string & level_final_label = config_.fetch_string("level_final");
          std::map<std::string, nuclear_level::handle_type>::const_iterator level_found = levels_->find(level_final_label);
          DT_THROW_IF(level_found == levels_->end(), std::logic_error,
                      "Cannot find final level '" << level_final_label << "' from dictionary of levels !");
          set_level_final(level_found->second.get());
        }
      }
    }

    if (! has_decay_id()) {
      if (config_.has_key("decay_id")) {
        const std::string & decay_id = config_.fetch_string("decay_id");
        set_decay_id(decay_id);
      }
    }
    DT_THROW_IF(! has_decay_id(), std::logic_error, "Missing decay id!");

    config_.export_starting_with(_auxiliaries_, "aux.");

    // Initialize:
    _init_decay_driver(config_);

    _initialized_ = true;
    return;
  }

  void nuclear_decay::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Decay is not initialized !");
    _initialized_ = false;
    _auxiliaries_.clear();
    if (_decay_driver_) {
      _decay_driver_.reset();
    }
    _decay_id_.clear();
    _set_default();
    return;
  }

  void nuclear_decay::_set_default()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    _decay_id_.clear();
    _level_initial_ = 0;
    _level_final_ = 0;
    //_external_decay_driver_ = 0;
    return;
  }

  void nuclear_decay::_init_decay_driver(const datatools::properties & config_)
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    // Configuration properties for the decay driver:
    datatools::properties decay_driver_config;
    config_.export_and_rename_starting_with(decay_driver_config, "driver.", "");
    if (_logging_ == datatools::logger::PRIO_TRACE) {
      decay_driver_config.tree_dump(std::cerr, "Decay driver config:", "TRACE: ");
    }
    base_decay_driver * driver = decay_driver_factory::create(_decay_id_);
    DT_THROW_IF (driver == 0, std::logic_error,
                 "Cannot create decay driver with id '" << _decay_id_ << "'!");
    driver->set_levels(*_level_initial_, *_level_final_);
    driver->initialize(decay_driver_config);
    _decay_driver_.reset(driver);

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  bool nuclear_decay::is_valid() const
  {
    if (_decay_id_.empty()) {
      return false;
    }
    return true;
  }

  bool nuclear_decay::has_decay_id() const
  {
    return ! _decay_id_.empty();
  }

  const std::string & nuclear_decay::get_decay_id() const
  {
    return _decay_id_;
  }

  void nuclear_decay::set_decay_id(const std::string & id_)
  {
    _decay_id_ = id_;
    return;
  }

  bool nuclear_decay::check_decay(std::string & message_) const
  {
    std::ostringstream om;
    if (! _level_initial_) {
      om << "Missing initial level !";
      message_ = om.str();
      return false;
    }
    if (! _level_initial_->is_valid()) {
      om << "Initial level is not valid!";
      message_ = om.str();
      return false;
    }
    if (! _level_final_) {
      om << "Missing final level !";
      message_ = om.str();
      return false;
    }
    if (_level_initial_ == _level_final_) {
      om << "Initial and final levels are the same!";
      message_ = om.str();
      return false;
    }
    if (! _level_final_->is_valid()) {
      om << "Final level is not valid!";
      message_ = om.str();
      return false;
    }
    if (_level_initial_->is_stable()) {
      om << "Initial level '" << _level_initial_->to_string()
         << "' is stable!";
      message_ = om.str();
      return false;
    }

    return true;
  }

  void nuclear_decay::set_level_initial(const nuclear_level & level_)
  {
    _level_initial_ = &level_;
    return;
  }

  const nuclear_level & nuclear_decay::get_level_initial() const
  {
    return *_level_initial_;
  }

  void nuclear_decay::set_level_final(const nuclear_level & level_)
  {
    _level_final_ = &level_;
    return;
  }

  const nuclear_level & nuclear_decay::get_level_final() const
  {
    return *_level_final_;
  }

  const datatools::properties & nuclear_decay::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties & nuclear_decay::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  bool nuclear_decay::has_decay_driver() const
  {
    return _decay_driver_;
  }

  base_decay_driver & nuclear_decay::grab_decay_driver()
  {
    return *_decay_driver_;
  }


  std::string nuclear_decay::to_string(unsigned int) const
  {
    std::ostringstream oss;
    if (is_valid()) {
      oss << _decay_id_
          << '['
          << _level_initial_->to_string()
          << "->"
          << _level_final_->to_string()
          << ']';
    } else {
      oss << "?";
    }
    return oss.str();
  }

  datatools::logger::priority nuclear_decay::get_logging() const
  {
    return _logging_;
  }

  void nuclear_decay::set_logging(datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }

  void nuclear_decay::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Decay Id    : '" << _decay_id_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Initial level : ";
    if (_level_initial_) {
      out_ << "'" << _level_initial_->to_string() << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Final level   : ";
    if (_level_final_) {
      out_ << "'" << _level_final_->to_string() << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Auxiliaries  : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent_ << i_tree_dumpable::skip_tag;
      _auxiliaries_.tree_dump(out_, "", indent_oss.str());
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Decay driver  : " << (_decay_driver_ ? "yes" : "no") << std::endl;
    if (_decay_driver_) {
      std::ostringstream indent2;
      indent2 << indent_ << i_tree_dumpable::skip_tag;
      _decay_driver_->tree_dump(out_, "", indent2.str());
    }

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Valid         : " << is_valid() << std::endl;

    return;
  }

} // end of namespace genbb
