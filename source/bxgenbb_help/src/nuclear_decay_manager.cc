// nuclear_decay_manager.cc
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
#include <genbb_help/nuclear_decay_manager.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>

// This project:
#include <genbb_help/nuclear_level.h>
#include <genbb_help/nuclear_decay.h>

namespace genbb {

  nuclear_decay_manager::nuclear_decay_manager()
  {
    _initialized_ = false;
    _logging_ = datatools::logger::PRIO_FATAL;
    _do_decay_tables_ = true;
    return;
  }

  nuclear_decay_manager::~nuclear_decay_manager()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void nuclear_decay_manager::load_level(const std::string & level_name_,
                                         const datatools::properties & level_config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Manager is already initialized !");
    DT_THROW_IF(_levels_.find(level_name_) != _levels_.end(),
                std::logic_error,
                "A level named '" << level_name_ << "' already exists !");
    {
      nuclear_level::handle_type dummy;
      _levels_[level_name_] = dummy;
    }
    nuclear_level::handle_type & hlevel = _levels_.find(level_name_)->second;
    hlevel.reset(new nuclear_level);
    nuclear_level & level = hlevel.grab();
    level.initialize(level_config_);
    return;
  }

  void nuclear_decay_manager::load_decay(const std::string & decay_name_,
                                         const datatools::properties & decay_config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Manager is already initialized !");
    DT_THROW_IF(_decays_.find(decay_name_) != _decays_.end(),
                std::logic_error,
                "A decay named '" << decay_name_ << "' already exists !");
    {
      nuclear_decay::handle_type dummy;
      _decays_[decay_name_] = dummy;
    }
    nuclear_decay::handle_type & hdecay = _decays_.find(decay_name_)->second;
    hdecay.reset(new nuclear_decay);
    nuclear_decay & decay = hdecay.grab();
    decay.initialize(decay_config_, &_levels_);
    return;
  }

  void nuclear_decay_manager::load_decay_table(const std::string & level_name_,
                                               const datatools::properties & decay_table_config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Manager is already initialized !");
    level_dict_type::iterator found = _levels_.find(level_name_);
    DT_THROW_IF(found == _levels_.end(),
                std::logic_error,
                "A level named '" << level_name_ << "' does not exist !");
    nuclear_level & level = found->second.grab();
    DT_THROW_IF(! level.is_initialized(),
                std::logic_error,
                "Level '" << level_name_ << "' is not initialized!");
    DT_THROW_IF(level.is_stable(),
                std::logic_error,
                "Level '" << level_name_ << "' is stable!");
    DT_THROW_IF(! level.get_decay_table_config().empty(),
                std::logic_error,
                "Level '" << level_name_ << "' already has a configuration for a decay table!");
    DT_THROW_IF(level.has_decay_table(),
                std::logic_error,
                "Level '" << level_name_ << "' already has a decay table!");
    decay_table_config_.export_all(level.grab_decay_table_config());
    return;
  }

  void nuclear_decay_manager::load(const std::string & filename_)
  {
    std::string filename = filename_;
    datatools::fetch_path_with_env(filename);
    datatools::multi_properties mconfig("name", "type");
    datatools::read_config(filename, mconfig);
    load(mconfig);
    return;
  }

  void nuclear_decay_manager::load(const datatools::multi_properties & levels_and_decays_config_)
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Manager is already initialized !");
    if (_logging_ >= datatools::logger::PRIO_TRACE) {
      levels_and_decays_config_.tree_dump(std::cerr, "Levels & decays & decay tables: ", "TRACE: ");
    }
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = levels_and_decays_config_.ordered_entries().begin();
         i != levels_and_decays_config_.ordered_entries().end();
         ++i) {
      datatools::multi_properties::entry * mpe = *i;
      const std::string & name = mpe->get_key();
      const std::string & type = mpe->get_meta();
      DT_LOG_TRACE(_logging_, "name='" << name << "' type='" << type << "'");
      if (type == "level") {
        this->load_level(name, mpe->get_properties());
      } else if (type == "decay") {
        this->load_decay(name, mpe->get_properties());
      } else if (type == "decay_table") {
        this->load_decay_table(name, mpe->get_properties());
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid type !");
      }
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  bool nuclear_decay_manager::is_initialized() const
  {
    return _initialized_;
  }

  void nuclear_decay_manager::initialize_simple()
  {
    datatools::properties dummy;
    initialize(dummy);
    return;
  }

  void nuclear_decay_manager::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Manager is already initialized !");

    _logging_ = datatools::logger::extract_logging_configuration(config_,
                                                                 datatools::logger::PRIO_FATAL,
                                                                 true);

    if (_logging_ >= datatools::logger::PRIO_TRACE) {
      config_.tree_dump(std::cerr, "Nuclear decay manager configuration: ", "TRACE: ");
    }
    if (config_.has_key("definition_files")) {
      std::vector<std::string> filenames;
      config_.fetch("definition_files", filenames);
      for (int i = 0; i < (int) filenames.size(); i++) {
        DT_LOG_TRACE(_logging_, "Loading file '" << filenames[i] << "'");
        // std::cerr << "DEVEL: "
        //           << "nuclear_decay_manager::initialize: "
        //           << "Loading file '" << filenames[i] << "'"
        //           << std::endl;
        load(filenames[i]);
      }
    }

    if (config_.has_key("decay_tables")) {
      _do_decay_tables_ = config_.fetch_boolean("decay_tables");
    }

    if (_do_decay_tables_) {
      _install_decay_tables();
    }

    _initialized_ = true;
    return;
  }

  void nuclear_decay_manager::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Manager is not initialized !");
    _initialized_ = false;
    _levels_.clear();
    _decays_.clear();
    _do_decay_tables_ = true;
    return;
  }

  const nuclear_decay_manager::level_dict_type &
  nuclear_decay_manager::get_levels() const
  {
    return _levels_;
  }

  const nuclear_decay_manager::decay_dict_type &
  nuclear_decay_manager::get_decays() const
  {
    return _decays_;
  }

  void nuclear_decay_manager::_install_decay_tables()
  {
    for (level_dict_type::iterator i = _levels_.begin();
         i != _levels_.end();
         i++) {
      nuclear_level & lvl = i->second.grab();
      if (! lvl.is_stable()) {
        DT_LOG_NOTICE(_logging_, "Install decay table in nuclear level '" <<  i->first << "'...");
        lvl.install_decay_table(lvl.get_decay_table_config(), _decays_);
      }
    }
    return;
  }

  datatools::logger::priority nuclear_decay_manager::get_logging() const
  {
    return _logging_;
  }

  void nuclear_decay_manager::set_logging(datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }

  void nuclear_decay_manager::tree_dump(std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent_,
                                        bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Levels : " << std::endl;
    for (level_dict_type::const_iterator i = _levels_.begin();
         i != _levels_.end();
         i++) {
      level_dict_type::const_iterator j = i;
      j++;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (j == _levels_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      const nuclear_level & lvl = i->second.get();
      out_ << "Level '" << i->first << "' : " << lvl.to_string();
      if (lvl.has_decay_table()) {
        out_ << " " << "(with decay table)";
      }
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Decays : " << std::endl;
    for (decay_dict_type::const_iterator i = _decays_.begin();
         i != _decays_.end();
         i++) {
      decay_dict_type::const_iterator j = i;
      j++;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (j == _decays_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Decay '" << i->first << "' : " << "with driver '"
           << i->second.get().to_string()
           << "'";
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Install decay tables: " << _do_decay_tables_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Initialized: " << is_initialized() << std::endl;

    return;
  }

} // end of namespace genbb
