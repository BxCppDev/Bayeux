// datatools/configuration/variant_registry_manager.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/configuration/variant_registry_manager.h>

// Standard library:
#include <fstream>

// This project (Bayeux/datatools):
#include <datatools/exception.h>
#include <datatools/multi_properties_config.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/parameter_model.h>

namespace datatools {

  namespace configuration {

    variant_registry_manager::variant_registry_manager()
    {
      _initialized_ = false;
      _set_default();
      return;
    }

    variant_registry_manager::~variant_registry_manager()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void variant_registry_manager::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Variant manager '" << get_name() << "' is not initialized!");
      _initialized_ = false;
      _configuration_items_.clear();
      _mp_.clear();
      _property_prefixes_.clear();
      _top_variant_name_.clear();
      _set_default();
      this->enriched_base::clear();
      return;
    }

    void variant_registry_manager::_set_default()
    {
      return;
    }

    bool variant_registry_manager::is_initialized() const
    {
      return _initialized_;
    }

    void variant_registry_manager::initialize_simple()
    {
      properties dummy_setup;
      initialize(dummy_setup);
      return;
    }

    void variant_registry_manager::initialize(const properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Variant manager '" << get_name() << "' is already locked !");

      // Configuration:
      enriched_base::initialize(config_);

      std::string              top_variant_name;
      std::vector<std::string> preserved_property_prefixes;
      std::string              configuration_items_list;
      std::vector<std::string> configuration_items_files;

      if (config_.has_key("top_variant_name")) {
        top_variant_name = config_.fetch_string("top_variant_name");
        set_top_variant_name(top_variant_name);
      }

      {
        /* Property prefixes to be preserved in configuration items */
        if (config_.has_key ("preserved_property_prefixes")) {
          config_.fetch ("preserved_property_prefixes", preserved_property_prefixes);
        }

        std::vector<std::string> default_preserved_property_prefixes;
        // default_preserved_property_prefixes.push_back("visibility");
        // default_preserved_property_prefixes.push_back("alarm");

        std::vector<std::string> effective_preserved_property_prefixes
          = default_preserved_property_prefixes;

        for (size_t i = 0; i < preserved_property_prefixes.size (); i++) {
          const std::string & prefix = preserved_property_prefixes[i];
          if (prefix.empty()) {
            continue;
          }
          if (std::find(effective_preserved_property_prefixes.begin(),
                        effective_preserved_property_prefixes.end(),
                        prefix) == effective_preserved_property_prefixes.end()) {
            effective_preserved_property_prefixes.push_back(prefix);
          }
        }

        for (size_t i = 0; i < effective_preserved_property_prefixes.size (); i++) {
          const std::string & prefix = effective_preserved_property_prefixes[i];
          DT_THROW_IF(prefix.empty(),
                      std::logic_error,
                      "Property prefix to be preserved in configuration item is empty !");
          DT_THROW_IF(prefix[prefix.length() - 1] != '.',
                      std::logic_error,
                      "Property prefix to be preserved in configuration item must end with a dot '.' ("
                      << prefix << " ) !");
          DT_LOG_DEBUG(get_logging_priority(), "Property prefix '" << prefix << "' will be exported");
          add_property_prefix(prefix);
        }
      }

      if (config_.has_key("configuration_items.list")) {
        configuration_items_list = config_.fetch_string("configuration_items.list");
      }

      if (config_.has_key ("configuration_items.files")) {
        config_.fetch("configuration_items.files", configuration_items_files);
      }

      // Initialization:

      if (! configuration_items_list.empty ()) {
        load_configuration_items_list(configuration_items_list);
      }

      for (std::vector<std::string>::const_iterator i
             = configuration_items_files.begin ();
           i != configuration_items_files.end ();
           i++) {
        std::string item_filename = *i;
        datatools::fetch_path_with_env(item_filename);
        DT_LOG_DEBUG(get_logging_priority(),
                     "Loading configuration item definition file '" << item_filename << "'... ");
        load(item_filename);
        DT_LOG_DEBUG(get_logging_priority(),
                     "Configuration items were loaded from file '" << item_filename << "'.");
      }

      _construct_();
      _mp_.reset();

      _initialized_ = true;
      return;
    }

    const std::string & variant_registry_manager::get_top_variant_name () const
    {
      return _top_variant_name_;
    }

    void variant_registry_manager::set_top_variant_name (const std::string & tvn_)
    {
      DT_THROW_IF(this->is_initialized(),
                  std::logic_error,
                  "Variant manager '" << get_name() << "' is already initialized !");
      _top_variant_name_ = tvn_;
      return;
    }

    void variant_registry_manager::add_property_prefix(const std::string & prefix_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Variant manager is locked !");
      DT_THROW_IF(prefix_.empty(),
                  std::logic_error,
                  "Prefix for properties to be preserved cannot be empty !");
      DT_THROW_IF(_property_prefixes_.count(prefix_) > 0,
                  std::logic_error,
                  "Prefix '" << prefix_<< "' for properties to be preserved already exists !");
      _property_prefixes_.insert(prefix_);
      return;
    }

    const std::set<std::string> & variant_registry_manager::get_property_prefixes() const
    {
      return _property_prefixes_;
    }

    void variant_registry_manager::load_configuration_items_list(const std::string & item_list_file_)
    {
      std::string item_lis_filename = item_list_file_;
      datatools::fetch_path_with_env(item_lis_filename);
      std::ifstream finlist(item_lis_filename.c_str());
      DT_THROW_IF(! finlist, std::logic_error,
                  "Cannot open file '" << item_lis_filename << "' !");
      while (finlist) {
        std::string line;
        std::getline(finlist, line);
        DT_THROW_IF(! finlist, std::logic_error,
                    "I/O error while reading file '" << item_lis_filename << "' !");
        std::string word;
        std::istringstream line_iss (line);
        line_iss >> word;
        if (word.length () < 1) {
          // skip blank line
          continue;
        }
        if (word[0] == '#') {
          continue;
        }
        std::string item_filename = word;
        datatools::fetch_path_with_env(item_filename);
        load(item_filename);
        finlist >> std::ws;
        if (finlist.eof()) {
          break;
        }
      }
      return;
    }

    void variant_registry_manager::load(const std::string & items_config_file_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Variant manager is locked !");
      std::string file_name = items_config_file_;
      uint32_t reader_opts = 0;
      reader_opts |= multi_properties_config::FORBID_VARIANTS;
      datatools::multi_properties::read_config(file_name, _mp_, reader_opts);
      // DT_LOG_TRACE(get_logging_priority(), "Variant manager multi-configuration: ");
      // _mp_.tree_dump(std::cerr,"", "TRACE: ");
      return;
    }

    const model_item_dict_type &variant_registry_manager::get_configuration_items() const
    {
      return _configuration_items_;
    }

    void variant_registry_manager::_construct_()
    {

      if (get_logging_priority() == logger::PRIO_TRACE) {
        _mp_.tree_dump(std::clog, "Configuration items definitions:", "[trace]: ");
      }

      bool test_top = true;
      if (test_top) {
        if (! _top_variant_name_.empty()) {
          DT_THROW_IF(! _mp_.has_section(_top_variant_name_), std::logic_error,
                      "No definition for configuration item named '" << _top_variant_name_ << "' !");
          const datatools::multi_properties::entry & top_variant_entry = _mp_.get(_top_variant_name_);
          const std::string & top_variant_type = top_variant_entry.get_meta();
          DT_THROW_IF(top_variant_type != model_item::label_from_model_type(model_item::MODEL_VARIANT),
                      std::logic_error,
                      "Definition for configuration item named '"
                      << _top_variant_name_ << "' does not correspond to a variant !");
        }
      }

      // Instantiate all configuration items:
      for (multi_properties::entries_ordered_col_type::const_iterator i = _mp_.ordered_entries().begin();
           i != _mp_.ordered_entries().end();
           i++) {
        const multi_properties::entry & mpe = **i;
        const std::string & item_name = mpe.get_key();
        const std::string & item_model = mpe.get_meta();
        const properties & item_config = mpe.get_properties();
        model_item::model_type type = model_item::model_type_from_label(item_model);
        /*
          model_item::model_type type = model_item::MODEL_UNDEFINED;
          if (item_model == "datatools::configuration::parameter_model") {
          type = model_item::MODEL_PARAMETER;
          } else if (item_model == "datatools::configuration::variant_model") {
          type = model_item::MODEL_VARIANT;
          }
        */
        DT_THROW_IF(type == model_item::MODEL_UNDEFINED, std::logic_error,
                    "Invalid model type '" << item_model << "' !");
        if (type == model_item::MODEL_VARIANT) {
          variant_model & a_variant = model_item::add_variant_item(_configuration_items_, item_name);
          a_variant.initialize(item_config, _configuration_items_);
        } else if (type == model_item::MODEL_PARAMETER) {
          parameter_model & a_parameter = model_item::add_parameter_item(_configuration_items_, item_name);
          a_parameter.initialize(item_config, _configuration_items_);
        }
      }

      return;
    }

    void variant_registry_manager::tree_dump(std::ostream& out_,
                                             const std::string& title_,
                                             const std::string& indent_,
                                             bool inherit_) const
    {
      this-> enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::tag
           << "Top variant name  : '"
           << _top_variant_name_ << "'" << std::endl;

      {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Definition for configuration model items : ";
        out_ << std::endl;
        for (multi_properties::entries_ordered_col_type::const_iterator i = _mp_.ordered_entries().begin();
             i != _mp_.ordered_entries().end();
             i++) {
          const multi_properties::entry & mpe = **i;
          multi_properties::entries_ordered_col_type::const_iterator j = i;
          if (++j == _mp_.ordered_entries().end()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "Name: '" << mpe.get_key() << "' with model: '" << mpe.get_meta() << "'";
          out_ << std::endl;
        }
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Prefixes for exported properties : "
           << std::endl;
      for (std::set<std::string>::const_iterator i = _property_prefixes_.begin();
           i != _property_prefixes_.end();
           i++) {
        out_ << datatools::i_tree_dumpable::skip_tag;
        std::set<std::string>::const_iterator j = i;
        if (++j == _property_prefixes_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        out_ << "Prefix : '" << *i << "'";
        out_ << std::endl;
      }

      {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Configuration model items : ";
        out_ << std::endl;
        std::ostringstream indent_ss;
        indent_ss << indent_ << datatools::i_tree_dumpable::skip_tag;
        model_item::print_items(_configuration_items_, out_, "", indent_ss.str());
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : "
           << this->is_initialized() << std::endl;

      return;
    }

    // static
    void variant_registry_manager::init_ocd(datatools::object_configuration_description & ocd_)
    {

      enriched_base::init_ocd(ocd_);

      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
