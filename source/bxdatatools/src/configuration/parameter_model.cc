// datatools/configuration/parameter_model.cc
/*
 * Copyright (C) 2014-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/configuration/parameter_model.h>

// Standard library:
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <unistd.h>

// Third party:
// - Boost:
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/regex.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/multi_properties.h>
#include <datatools/ioutils.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/variant_physical.h>
#include <datatools/configuration/variant_repository.h>

namespace datatools {

  namespace configuration {

    // virtual
    bool parameter_model::is_name_valid(const std::string & name_) const
    {
      // std::cerr << "DEVEL: parameter_model::is_name_valid: name = '" << name_ << "'" << std::endl;
      return ::datatools::configuration::validate_model_name(name_);
    }

    // static
    std::string parameter_model::label_from_mutability(const mutability_type mutability_)
    {
      switch (mutability_) {
      case MUTABILITY_FIXED :
        return "fixed";
      case MUTABILITY_VARIABLE :
        return "variable";
      default:
        return "";
      }
    }

    // static
    parameter_model::mutability_type parameter_model::mutability_from_label(const std::string & ml_)
    {
      if (ml_ == "fixed") {
        return MUTABILITY_FIXED;
      } else if (ml_ == "variable") {
        return MUTABILITY_VARIABLE;
      }
      return MUTABILITY_UNDEFINED;
    }

    // static
    std::string parameter_model::label_from_variable_mode(const variable_mode_type variable_mode_)
    {
      switch (variable_mode_) {
      case VARIABLE_MODE_FREE :
        return "free";
      case VARIABLE_MODE_ENUM :
        return "enumeration";
      case VARIABLE_MODE_INTERVAL :
        return "interval";
      case VARIABLE_MODE_DEFAULT :
        return "default";
      default:
        return "";
      }
    }

    // static
    parameter_model::variable_mode_type parameter_model::variable_mode_from_label(const std::string & label_)
    {
      if (label_ == label_from_variable_mode(VARIABLE_MODE_FREE)) {
        return VARIABLE_MODE_FREE;
      } else if (label_ == label_from_variable_mode(VARIABLE_MODE_ENUM)) {
        return VARIABLE_MODE_ENUM;
      } else if (label_ == label_from_variable_mode(VARIABLE_MODE_INTERVAL)) {
        return VARIABLE_MODE_INTERVAL;
      } else if (label_ == label_from_variable_mode(VARIABLE_MODE_DEFAULT)) {
        return VARIABLE_MODE_DEFAULT;
      } else {
        return VARIABLE_MODE_UNDEFINED;
      }
    }

    // static
    std::string
    parameter_model::label_from_path_io(const path_io_type path_io_)
    {
      switch (path_io_) {
      case PATH_INPUT :
        return "input";
      case PATH_OUTPUT :
        return "output";
      case PATH_INPUT_OUTPUT :
        return "input/output ";
      default:
        return "";
      }
    }

    // static
    parameter_model::path_io_type
    parameter_model::path_io_from_label(const std::string & label_)
    {
      if (label_ == label_from_path_io(PATH_INPUT)) {
        return PATH_INPUT;
      } else if (label_ == label_from_path_io(PATH_OUTPUT)) {
        return PATH_OUTPUT;
      } else if (label_ == label_from_path_io(PATH_INPUT_OUTPUT)) {
        return PATH_INPUT_OUTPUT;
      } else {
        return PATH_UNDEFINED;
      }
    }

    // static
    const std::string & parameter_model::aux_prefix()
    {
      static std::string _prefix("aux.");
      return _prefix;
    }

    bool parameter_model::base_enum_metadata::has_group_support() const
    {
      return !_no_group_support_;
    }

    parameter_model::base_enum_metadata::base_enum_metadata(uint32_t flags_)
    {
      _rank = RANK_DEFAULT; 
      _no_group_support_ = false;
      if (flags_ & CTOR_NO_GROUP_SUPPORT) {
        _no_group_support_ = true;
      }
      return;
    }

    bool parameter_model::base_enum_metadata::has_documentation() const
    {
      return !_documentation.empty();
    }

    void parameter_model::base_enum_metadata::set_documentation(const std::string & doc_)
    {
      _documentation = doc_;
      return;
    }

    const std::string & parameter_model::base_enum_metadata::get_documentation() const
    {
      DT_THROW_IF(_documentation.empty(), std::logic_error, "No documentation!");
      return _documentation;
    }

    bool parameter_model::base_enum_metadata::has_variants() const
    {
      return _variants.size() > 0;
    }

    bool parameter_model::base_enum_metadata::has_variant(const std::string & variant_name_) const
    {
      return _variants.count(variant_name_) == 1 ;
    }

    void parameter_model::base_enum_metadata::add_variant(const std::string & variant_name_)
    {
      _variants.insert(variant_name_);
      return;
    }

    void parameter_model::base_enum_metadata::remove_variant(const std::string & variant_name_)
    {
      _variants.erase(variant_name_);
      return;
    }

    void parameter_model::base_enum_metadata::remove_variants()
    {
      _variants.clear();
      return;
    }

    const std::set<std::string> & parameter_model::base_enum_metadata::get_variants() const
    {
      return _variants;
    }

    void parameter_model::base_enum_metadata::set_rank(int rank_)
    {
      DT_THROW_IF(rank_ < 0 or rank_ > RANK_LAST, std::domain_error,
                  "Rank [" << std::to_string(rank_) << "] is not valid!");
      _rank = rank_;
      return;
    }

    int parameter_model::base_enum_metadata::get_rank() const
    {
      return _rank;
    }

    bool parameter_model::base_enum_metadata::has_group() const
    {
      return !_group.empty();
    }

    void parameter_model::base_enum_metadata::set_group(const std::string & group_name_)
    {
      _group = group_name_;
      return;
    }

    bool parameter_model::base_enum_metadata::match_group(const std::string & group_name_) const
    {
      if (group_name_.empty()) {
        if (_group.empty()) {
          return true;
        }
        return false;
      }
      if (_group != group_name_) return false;
      return true;
    }

    const std::string & parameter_model::base_enum_metadata::get_group() const
    {
      DT_THROW_IF(_group.empty(), std::logic_error, "No group!");
      return _group;
    }

    void parameter_model::base_enum_metadata::reset()
    {
      _documentation.clear();
      _variants.clear();
      _group.clear();
      return;
    }

    void parameter_model::base_enum_metadata::initialize(const datatools::properties & config_,
                                                         parameter_model & parmod_)
    {
      // DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);

      // Fetch documentation:
      if (config_.has_key("documentation")) {
        const std::string & doc = config_.fetch_string("documentation");
        DT_THROW_IF(doc.empty(), std::logic_error, "Invalid empty documentation!");
        set_documentation(doc);
      }

      if (has_group_support()) {
        // Fetch group:
        // DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
        //              "Fetching groups for parameter model '" << parmod_.get_name() << "'...");
        if (config_.has_key("group")) {
          const std::string & group_name = config_.fetch_string("group");
          DT_THROW_IF(group_name.empty(), std::logic_error, "Invalid empty group name!");
          set_group(group_name);
          if (!parmod_.has_group(group_name)) {
            parmod_.add_group(group_name);
          }
        }
      }

      // Fetch associated variants:
      {
        // DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
        //              "Fetching associated variants for parameter model '" << parmod_.get_name() << "'...");
        std::set<std::string> variants_names;
        if (config_.has_key("variants")) {
          // A set of variants:
          config_.fetch("variants", variants_names);
        } else if (config_.has_key("variant")) {
          // A single variant (usual case):
          const std::string & variant_name = config_.fetch_string("variant");
          // DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
          //              "Found single variant '" << variant_name << "' for parameter model '" << parmod_.get_name() << "'");
          DT_THROW_IF(variant_name.empty(), std::logic_error, "Invalid empty variant name!");
          variants_names.insert(variant_name);
        }
        // Check if variants exist:
        for (const auto & variant_name : variants_names) {
          DT_THROW_IF(!parmod_.has_variant(variant_name), std::logic_error,
                      "Parameter model has no variant name '" << variant_name << "'!");
          add_variant(variant_name);
        }
      }

      // DT_LOG_TRACE_EXITING(datatools::logger::PRIO_TRACE);
      return;
    }

    void parameter_model::base_enum_metadata::print_rst(std::ostream & out_,
                                                        const std::string & indent_,
                                                        const uint32_t flags_) const
    {
      std::ostringstream out;
      bool has_content = false;
      std::string indent = indent_;

      bool with_doc = true;
      if (flags_ & PRINT_RST_NO_DOC) {
        with_doc = false;
      }

      if (with_doc && has_documentation()) {
        out << indent;
        out << "*" << get_documentation() << "*";
        out << std::endl;
        has_content = true;
      }

      out << std::endl;

      if (_variants.size() > 0) {
        if (_variants.size() == 1) {
          out << indent_ << "* Triggered variant : ";
          out << "``\"" << *_variants.begin() << "\"``";
          out << std::endl;
        } else {
          out << indent_ << "* Triggered variants : " << std::endl;
          out << indent_ << std::endl;
          for (const auto & varname : _variants) {
            out << indent_ << "  * ``\"" << varname << "\"``" << std::endl;
          }
          out << indent_ << std::endl;
        }
        has_content = true;
      }

      if (has_group_support()) {
        if (has_group()) {
          out << indent_ << "* Group : ``\"" << _group << "\"``"  << std::endl;
          has_content = true;
        }
      }

      out << std::endl;

      if (has_content) {
       out_ << out.str();
      }
      return;
    }

    void parameter_model::base_enum_metadata::tree_dump(std::ostream & out_,
                                                        const std::string & title_,
                                                        const std::string & indent_,
                                                        bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Documentation  : ";
      if (has_documentation()) {
        out_ << "'" << _documentation << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Variants  : ";
      if (_variants.size() == 0) {
        out_ << "<none>";
      }
      out_ << std::endl;
      {
        std::size_t variant_counter = 0;
        for (const auto & varname : _variants) {
          out_ << indent_ << i_tree_dumpable::skip_tag;
          if (++variant_counter == _variants.size()) {
            out_ << i_tree_dumpable::last_tag;
          } else {
            out_ << i_tree_dumpable::tag;
          }
          out_ << "Variant : '" << varname << "'" << std::endl;
        }
      }

      out_ << indent_ << i_tree_dumpable:: i_tree_dumpable::inherit_tag(inherit_)
           << "Group  : ";
      if (has_group()) {
        out_ << "'" << _group << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      return;
    }

    parameter_model::boolean_enum_value_metadata::boolean_enum_value_metadata()
      : base_enum_metadata(CTOR_NO_GROUP_SUPPORT)
    {
      return;
    }

    parameter_model::parameter_model()
    {
      _initialized_ = false;
      _type_ = TYPE_NONE;
      _variable_mode_ = VARIABLE_MODE_UNDEFINED;
      _set_default();
      return;
    }

    parameter_model::~parameter_model()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    bool parameter_model::has_documentation() const
    {
      return ! _documentation_.empty();
    }

    void parameter_model::set_documentation(const std::string & documentation_)
    {
      _documentation_ = documentation_;
      return;
    }

    const std::string & parameter_model::get_documentation() const
    {
      return _documentation_;
    }

    bool parameter_model::has_type() const
    {
      return _type_ != TYPE_NONE;
    }

    void parameter_model::set_type(const basic_type type_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      _type_ = type_;
      return;
    }

    basic_type parameter_model::get_type() const
    {
      return _type_;
    }

    bool parameter_model::is_boolean() const
    {
      return _type_ == TYPE_BOOLEAN;
    }

    bool parameter_model::is_integer() const
    {
      return _type_ == TYPE_INTEGER;
    }

    bool parameter_model::is_real() const
    {
      return _type_ == TYPE_REAL;
    }

    bool parameter_model::is_string() const
    {
      return _type_ == TYPE_STRING;
    }

    bool parameter_model::is_path() const
    {
      return is_string() && _path_;
    }

    bool parameter_model::has_path_io() const
    {
      return _path_io_ != PATH_UNDEFINED;
    }

    bool parameter_model::is_path_input() const
    {
      return is_path() && (_path_io_ & PATH_INPUT);
    }

    bool parameter_model::is_path_output() const
    {
      return is_path() && (_path_io_ & PATH_OUTPUT);
    }

    bool parameter_model::is_path_input_output() const
    {
      return is_path() && (_path_io_ & PATH_INPUT_OUTPUT);
    }

    void parameter_model::set_path_io(const path_io_type io_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      _path_io_ = io_;
      return;
    }

    void parameter_model::reset_path()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      _path_ = false;
      return;
    }

    void parameter_model::set_path(const bool p_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (!has_type()) {
        set_type(TYPE_STRING);
      } else {
        DT_THROW_IF(!is_string(), std::logic_error, "Parameter model '" << get_name() << "' is not of string type!");
      }
      _path_ = p_;
      return;
    }

    void parameter_model::set(const basic_type type_, const variable_mode_type variable_mode_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      set_type(type_);
      if (variable_mode_ != VARIABLE_MODE_UNDEFINED) {
        set_mutability(MUTABILITY_VARIABLE);
        _set_variable_mode(variable_mode_);
      } else {
        set_mutability(MUTABILITY_FIXED);
      }
      return;
    }

    bool parameter_model::has_variable_mode() const
    {
      return _variable_mode_ != VARIABLE_MODE_UNDEFINED;
    }

    void parameter_model::set_variable_mode(const variable_mode_type variable_mode_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(!has_type(),
                  std::logic_error,
                  "Parameter model '" << get_name() << "' has no valid type!");
      if (! has_mutability()) {
        _mutability_ = MUTABILITY_VARIABLE;
      }
      DT_THROW_IF(! is_variable(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not variable!");
      _set_variable_mode(variable_mode_);
      return;
    }

    parameter_model::variable_mode_type parameter_model::get_variable_mode() const
    {
      return _variable_mode_;
    }

    bool parameter_model::has_mutability() const
    {
      return _mutability_ != MUTABILITY_UNDEFINED;
    }

    void parameter_model::set_mutability(const mutability_type mutability_)
    {
      _mutability_ = mutability_;
      return;
    }

    parameter_model::mutability_type parameter_model::get_mutability() const
    {
      return _mutability_;
    }

    bool parameter_model::is_fixed() const
    {
      return _mutability_ == MUTABILITY_FIXED;
    }

    bool parameter_model::is_variable() const
    {
      return _mutability_ == MUTABILITY_VARIABLE;
    }

    bool parameter_model::has_fixed_value() const
    {
      DT_THROW_IF(!is_fixed(), std::logic_error, "Not a fixed parameter model!");
      return _has_fixed_value_;
    }

    bool parameter_model::get_fixed_boolean() const
    {
      DT_THROW_IF(! is_fixed(), std::logic_error, "Not a fixed boolean parameter_model!");
      return _boolean_fixed_;
    }

    int parameter_model::get_fixed_integer() const
    {
      DT_THROW_IF(! is_fixed(), std::logic_error, "Not a fixed integer parameter_model!");
      return _integer_fixed_;
    }

    double parameter_model::get_fixed_real() const
    {
      DT_THROW_IF(! is_fixed(), std::logic_error, "Not a fixed real parameter_model!");
      return _real_fixed_;
    }

    const std::string & parameter_model::get_fixed_string() const
    {
      DT_THROW_IF(! is_fixed(), std::logic_error, "Not a fixed string parameter_model!");
      return _string_fixed_;
    }

    void parameter_model::set_fixed_boolean(const bool value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_BOOLEAN);
      }
      DT_THROW_IF(! is_boolean(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of boolean type!");
      if (! has_mutability()) {
        set_mutability(MUTABILITY_FIXED);
      }
      DT_THROW_IF(!is_fixed(), std::logic_error, "Parameter model '" << get_name() << "' is not a fixed boolean parameter_model!");
      _boolean_fixed_ = value_;
      _has_fixed_value_ = true;
      return;
    }

    void parameter_model::set_fixed_integer(const int value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_INTEGER);
      }
      DT_THROW_IF(! is_integer(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of integer type!");
      if (! has_mutability()) {
        set_mutability(MUTABILITY_FIXED);
      }
      DT_THROW_IF(!is_fixed(), std::logic_error, "Parameter model '" << get_name() << "' is not a fixed integer parameter_model!");
      _integer_fixed_ = value_;
      _has_fixed_value_ = true;
      return;
    }

    void parameter_model::set_fixed_real(const double value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_REAL);
      }
      DT_THROW_IF(! is_real(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of string type!");
      if (! has_mutability()) {
        set_mutability(MUTABILITY_FIXED);
      }
      DT_THROW_IF(!is_fixed(), std::logic_error, "Parameter model '" << get_name() << "' is not a fixed real parameter_model!");
      _real_fixed_ = value_;
      _has_fixed_value_ = true;
      return;
    }

    void parameter_model::set_fixed_string(const std::string & value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_STRING);
      }
      DT_THROW_IF(! is_string(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of string type!");
      if (! has_mutability()) {
        set_mutability(MUTABILITY_FIXED);
      }
      DT_THROW_IF(!is_fixed(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not a fixed string parameter_model!");
      _string_fixed_ = value_;
      _has_fixed_value_ = true;
      return;
    }

    bool parameter_model::is_free() const
    {
      return _variable_mode_ == VARIABLE_MODE_FREE;
    }

    bool parameter_model::is_interval() const
    {
      return _variable_mode_ == VARIABLE_MODE_INTERVAL;
    }

    bool parameter_model::is_enumeration() const
    {
      return _variable_mode_ == VARIABLE_MODE_ENUM;
    }

    bool parameter_model::has_integer_domain() const
    {
      return _integer_domain_.is_valid();
    }

    void parameter_model::set_integer_domain(const integer_range & ir_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set(TYPE_INTEGER, VARIABLE_MODE_INTERVAL);
      }
      DT_THROW_IF(! is_integer(), std::logic_error, "Parameter model '" << get_name() << "' is not of integer type!");
      if (_variable_mode_ == VARIABLE_MODE_UNDEFINED) {
        _set_variable_mode(VARIABLE_MODE_INTERVAL);
      }
      DT_THROW_IF(! is_interval(),  std::logic_error,
                  "Parameter model '" << get_name() << "' does not use interval variable mode!");
      DT_THROW_IF(! ir_.is_valid(),  std::logic_error,
                  "Parameter model '" << get_name() << "' : Invalid interval !");
      _integer_domain_ = ir_;
      return;
    }

    const integer_range & parameter_model::get_integer_domain() const
    {
      DT_THROW_IF(! is_integer(), std::logic_error, "Not an integer parameter_model!");
      DT_THROW_IF(! is_variable(), std::logic_error, "Not a mutable parameter_model!");
      DT_THROW_IF(! is_interval(), std::logic_error, "Not an interval parameter_model!");
      return _integer_domain_;
    }

    bool parameter_model::has_real_domain() const
    {
      return _real_domain_.is_valid();
    }

    void parameter_model::set_real_domain(const real_range & rr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_REAL);
      }
      DT_THROW_IF(! is_real(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of real type!");

      if (! has_mutability()) {
        set_mutability(MUTABILITY_VARIABLE);
      }
      DT_THROW_IF(! is_variable(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not variable!");

      if (! has_variable_mode()) {
        set_variable_mode(VARIABLE_MODE_INTERVAL);
      }
      DT_THROW_IF(! is_interval(),  std::logic_error,
                  "Parameter model '" << get_name() << "' does not use interval variable mode!");

      DT_THROW_IF(! rr_.is_valid(),  std::logic_error,
                  "Parameter model '" << get_name() << "' : Invalid domain!");

      if (rr_.has_unit_label()) {
        DT_THROW_IF(!has_real_unit_label(),  std::logic_error,
                  "Parameter model '" << get_name() << "' does not have an unit label!");
        DT_THROW_IF(rr_.get_unit_label() != get_real_unit_label(),  std::logic_error,
                  "does not match unit label for parameter model '" << get_name() << "' !");
      } else {
        DT_THROW_IF(has_real_unit_label(),  std::logic_error,
                  "Parameter model '" << get_name() << "' has an unit label!");
      }

      datatools::invalidate(_real_default_);
      _real_ranges_.clear();
      _real_domain_ = rr_;
      return;
    }

    const real_range & parameter_model::get_real_domain() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not an real parameter model!");
      DT_THROW_IF(! is_variable(), std::logic_error, "Not a mutable parameter model!");
      DT_THROW_IF(! is_interval(), std::logic_error, "Not an interval parameter model!");
      return _real_domain_;
    }

    bool parameter_model::has_default_value() const
    {
      DT_THROW_IF(is_fixed(), std::logic_error, "Not a mutable parameter model!");
      return _has_default_value_;
    }

    void parameter_model::reset_default_value()
    {
      _has_default_value_ = false;
      _boolean_default_ = false;
      _integer_default_ = 0;
      datatools::invalidate(_real_default_);
      _string_default_.clear();
      return;
    }

    void parameter_model::set_default_boolean(const bool value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_BOOLEAN);
      }
      DT_THROW_IF(! is_boolean(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of boolean type!");
      if (_mutability_ == MUTABILITY_UNDEFINED) {
        set_mutability(MUTABILITY_VARIABLE);
      }
      DT_THROW_IF(is_fixed(), std::logic_error, "Parameter model '" << get_name() << "' is not a mutable boolean parameter model!");
      _boolean_default_ = value_;
      _has_default_value_ = true;
      return;
    }

    bool parameter_model::get_default_boolean() const
    {
      DT_THROW_IF(!has_default_value(), std::logic_error, "Has no default boolean value!");
      return _boolean_default_;
    }

    void parameter_model::set_default_integer(const int value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_INTEGER);
      }
      DT_THROW_IF(! is_integer(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of integer type!");
      if (_mutability_ == MUTABILITY_UNDEFINED) {
        set_mutability(MUTABILITY_VARIABLE);
      }
      DT_THROW_IF(is_fixed(), std::logic_error, "Parameter model '" << get_name() << "' is not a mutable integer parameter model!");
      if (is_free()) {
        _integer_default_ = value_;
        _has_default_value_ = true;
      } else if (is_interval()) {
        DT_THROW_IF(! _integer_domain_.has(value_),
                    std::logic_error,
                    "Default integer value for parameter model '" << get_name()
                    << "' is not in the accepted domain " << _integer_domain_ << " !");
        _integer_default_ = value_;
        _has_default_value_ = true;
      } else if (is_enumeration()) {
        DT_THROW_IF(_integer_enumeration_.find(value_) == _integer_enumeration_.end(), std::range_error,
                    "Default integer value '" << value_ << "' for parameter model '"
                    << get_name() << "' is not in the supported enumeration !");
        _integer_default_ = value_;
        _has_default_value_ = true;
      }
      return;
    }

    int parameter_model::get_default_integer() const
    {
      DT_THROW_IF(!has_default_value(), std::logic_error, "Has no default integer value!");
      return _integer_default_;
    }

    bool parameter_model::has_real_precision() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not a real parameter_model!");
      return std::isfinite(_real_precision_);
    }

    void parameter_model::set_real_precision(const double prec_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_REAL);
      }
      DT_THROW_IF(! is_real(), std::logic_error, "Parameter model '" << get_name() << "' is not of real type!");
      DT_THROW_IF(!std::isfinite(prec_), std::domain_error,
                  "Invalid precision value for real parameter model '" << get_name() << "'!");
      DT_THROW_IF(prec_ < 0.0, std::domain_error,
                  "Real parameter model '" << get_name() << "' cannot use a negative precision in interval/enumeration variable mode!");
      _real_precision_ = prec_;
      if (is_variable() && is_interval()) {
        _update_real_domain_();
      }
      return;
    }

    void parameter_model::reset_real_precision()
    {
      datatools::invalidate(_real_precision_);
      return;
    }

    double parameter_model::get_real_effective_precision() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not a real parameter_model!");
      if (std::isfinite(_real_precision_)) return _real_precision_;
      return std::numeric_limits<double>::epsilon();
    }

    double parameter_model::get_real_precision() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not a real parameter_model!");
      return _real_precision_;
    }

    void parameter_model::set_default_real(const double value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_REAL);
      }
      DT_THROW_IF(! is_real(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of real type!");
      if (_mutability_ == MUTABILITY_UNDEFINED) {
        set_mutability(MUTABILITY_VARIABLE);
      }
      DT_THROW_IF(is_fixed(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not a mutable real parameter model!");
      if (is_free()) {
        _real_default_ = value_;
        _has_default_value_ = true;
      } else if (is_interval()) {
        DT_THROW_IF(! _real_domain_.has(value_, get_real_precision()),
                    std::logic_error,
                    "Default real value for parameter model '" << get_name()
                    << "' is not in the accepted domain " << _real_domain_ << " !");
        _real_default_ = value_;
        _has_default_value_ = true;
      } else if (is_enumeration()) {
        double value;
        bool found = find_enumerated_real_value(value_, value);
        DT_THROW_IF(!found, std::range_error,
                    "Default logic value '" << value_ << "' for parameter model '"
                    << get_name() << "' is not in the supported enumeration !");
        _real_default_ = value;
        _has_default_value_ = true;
      }
      return;
    }

    double parameter_model::get_default_real() const
    {
      DT_THROW_IF(!has_default_value(), std::logic_error, "Has no default real value!");
      return _real_default_;
    }

    void parameter_model::set_default_string(const std::string & value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_STRING);
      }
      DT_THROW_IF(! is_string(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of string type!");
      if (_mutability_ == MUTABILITY_UNDEFINED) {
        set_mutability(MUTABILITY_VARIABLE);
      }
      DT_THROW_IF(is_fixed(), std::logic_error, "Parameter model '" << get_name() << "' is not a mutable string parameter model!");
      if (is_free()) {
        _string_default_ = value_;
        _has_default_value_ = true;
      } else if (is_enumeration()) {
        // DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Parameter model = '" << get_name() << "' : ");
        // for (const auto & sep : _string_enumeration_) {
        //   DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "  --> Supported enumeration string value = '" << sep.first << "'");
        // }
        DT_THROW_IF(_string_enumeration_.find(value_) == _string_enumeration_.end(), std::range_error,
                    "Default logic value '" << value_ << "' for parameter model '" << get_name() << "' is not in the supported enumeration !");
        _string_default_ = value_;
        _has_default_value_ = true;
      }
      return;
    }

    const std::string & parameter_model::get_default_string() const
    {
      DT_THROW_IF(!has_default_value(), std::logic_error, "Has no default string value!");
      return _string_default_;
    }

    bool parameter_model::has_real_unit_label() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not real parameter model '" << get_name() << "' does not support unit !");
      return ! _real_unit_label_.empty();
    }


    void parameter_model::set_real_unit_label(const std::string & unit_label_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Description of parameter model '" << get_name() << "' is locked!");
      if (! has_type()) {
        set_type(TYPE_REAL);
      }
      DT_THROW_IF(! is_real(), std::logic_error, "Not a real parameter model '" << get_name() << "' !");
      DT_THROW_IF(! units::is_unit_label_valid(unit_label_), std::logic_error,
                  "Unit label '" << unit_label_ << "' is not supported !");
      if (has_real_preferred_unit()) {
        DT_THROW_IF(unit_label_ != _real_preferred_unit_, std::logic_error,
                    "Unit label '" << unit_label_ << "' is not compatible with current unit symbol '"
                    << _real_preferred_unit_ << "' !");
      }
      _real_unit_label_ = unit_label_;
      if (is_variable() && is_interval()) {
        _update_real_domain_();
      }
      return;
    }


    const std::string &parameter_model::get_real_unit_label() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not real parameter model '" << get_name() << "' does not support unit !");
      return _real_unit_label_;
    }


    bool parameter_model::has_real_preferred_unit() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not real parameter model '" << get_name() << "' does not support unit !");
      return ! _real_preferred_unit_.empty();
    }


    void parameter_model::set_real_preferred_unit(const std::string & unit_symbol_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_real(), std::logic_error, "Not real parameter model '" << get_name() << "' does not support unit !");
      {
        double uv;
        std::string ul;
        DT_THROW_IF(! units::find_unit(unit_symbol_, uv, ul), std::logic_error,
                    "Unit symbol '" << unit_symbol_ << "' is not supported !");
        if (has_real_unit_label()) {
          DT_THROW_IF(! units::find_unit(unit_symbol_, uv, ul), std::logic_error,
                      "Unit symbol '" << unit_symbol_ << "' does not match the requested unit label '"
                      << ul << "' !");
        } else {
          set_real_unit_label(ul);
        }
      }
      _real_preferred_unit_ = unit_symbol_;
      if (is_real() && is_variable() && is_interval()) {
        _update_real_domain_();
      }
      return;
    }

    const std::string & parameter_model::get_real_preferred_unit() const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Not real parameter model '" << get_name() << "' does not support unit !");
      return _real_preferred_unit_;
    }

    bool parameter_model::fetch_real_display_unit(std::string & display_unit_symbol_,
                                                  double & unit_value_) const
    {
      display_unit_symbol_.clear();
      if (has_real_preferred_unit()) {
        display_unit_symbol_ = get_real_preferred_unit();
        std::string unit_label;
        if (units::find_unit(display_unit_symbol_, unit_value_, unit_label)) {
          return true;
        }
      }
      if (has_real_unit_label()) {
        display_unit_symbol_ = units::get_default_unit_symbol_from_label(get_real_unit_label());
        std::string unit_label;
        if (units::find_unit(display_unit_symbol_, unit_value_, unit_label)) {
          return true;
        }
      }
      return false;
    }

    // Boolean

    void parameter_model::set_enumerated_boolean_value_metadata(const bool value_,
                                                                const boolean_enum_value_metadata & metadata_)
    {
      if (value_) _true_metadata_ = metadata_;
      else _false_metadata_ = metadata_;
      return;
    }

    const parameter_model::boolean_enum_value_metadata &
    parameter_model::get_enumerated_boolean_value_metadata(const bool value_) const
    {
      if (value_) return _true_metadata_;
      return _false_metadata_;
    }

    parameter_model::boolean_enum_value_metadata &
    parameter_model::grab_enumerated_boolean_value_metadata(const bool value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      if (value_) return _true_metadata_;
      return _false_metadata_;
    }

    bool parameter_model::enumerated_boolean_value_has_group(const bool value_) const
    {
      if (value_) {
        return _true_metadata_.has_group();
      } else {
        return _false_metadata_.has_group();
      }
    }

    const std::string & parameter_model::get_enumerated_boolean_value_group(const bool value_) const
    {
      if (value_) {
        DT_THROW_IF(!_true_metadata_.has_group(),
                    std::logic_error,
                    "Value '" << value_ << "' has no associated group!");
        return _true_metadata_.get_group();
      } else {
        DT_THROW_IF(!_false_metadata_.has_group(),
                    std::logic_error,
                    "Value '" << value_ << "' has no associated group!");
        return _false_metadata_.get_group();
      }
    }

    void parameter_model::build_list_of_enumerated_boolean_groups(std::set<std::string> & groups_) const
    {
      groups_.clear();
      if (_true_metadata_.has_group()) {
         groups_.insert(_true_metadata_.get_group());
      }
      if (_false_metadata_.has_group()) {
         groups_.insert(_false_metadata_.get_group());
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_boolean_values_in_group(const std::string & group_,
                                                                           std::set<bool> & values_) const
    {
      values_.clear();
      if (_true_metadata_.match_group(group_)) {
         values_.insert(true);
      }
      if (_false_metadata_.match_group(group_)) {
         values_.insert(false);
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_boolean_values(std::set<bool> & values_) const
    {
      values_.clear();
      values_.insert(true);
      values_.insert(false);
      return;
    }

    // Integer enumerated

    bool parameter_model::has_enumerated_integer_value(const int value_) const
    {
      return _integer_enumeration_.find(value_) != _integer_enumeration_.end();
    }

    void parameter_model::add_enumerated_integer_value(const int value_,
                                                       const bool default_)
    {
      integer_enum_value_metadata null;
      add_enumerated_integer_value(value_, null, default_);
      return;
    }

    void parameter_model::add_enumerated_integer_value(const int value_,
                                                       const integer_enum_value_metadata & metadata_,
                                                       const bool default_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_integer(), std::logic_error, "Parameter model '" << get_name() << "' is not of integer type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
      DT_THROW_IF(has_enumerated_integer_value(value_),
                  std::logic_error,
                  "Integer parameter model '" << get_name() << "' "
                  << "already has enumeration value '" << value_ << "' !");
      _integer_enumeration_[value_] = metadata_;
      if (default_) {
        set_default_integer(value_);
      }
      return;
    }

    void parameter_model::fetch_integer_enumeration(std::vector<int> & ienums_) const
    {
      DT_THROW_IF(! is_integer(), std::logic_error, "Parameter model '" << get_name() << "' is not of integer type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
      ienums_.clear();
      for (integer_enum_dict_type::const_iterator i = _integer_enumeration_.begin();
           i != _integer_enumeration_.end();
           i++) {
        ienums_.push_back(i->first);
      }
      return;
    }

    parameter_model::integer_enum_value_metadata &
    parameter_model::grab_enumerated_integer_value_metadata(const int value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_integer(), std::logic_error, "Parameter model '" << get_name() << "' is not of integer type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
      integer_enum_dict_type::iterator found = _integer_enumeration_.find(value_);
      DT_THROW_IF(found == _integer_enumeration_.end(),
                  std::logic_error,
                  "Integer parameter model '" << get_name() << "' has no enumerated value '" << value_ << "'!");
      return found->second;
    }

    const parameter_model::integer_enum_value_metadata &
    parameter_model::get_enumerated_integer_value_metadata(const int value_) const
    {
      DT_THROW_IF(! is_integer(), std::logic_error, "Parameter model '" << get_name() << "' is not of integer type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
      integer_enum_dict_type::const_iterator found = _integer_enumeration_.find(value_);
      DT_THROW_IF(found == _integer_enumeration_.end(),
                  std::logic_error,
                  "Integer parameter model '" << get_name() << "' has no enumerated value '" << value_ << "'!");
      return found->second;
    }

    bool parameter_model::enumerated_integer_value_has_group(const int value_) const
    {
      DT_THROW_IF(! is_integer(), std::logic_error, "Parameter model '" << get_name() << "' is not of integer type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
       const auto & found = _integer_enumeration_.find(value_);
      DT_THROW_IF(found == _integer_enumeration_.end(),
                  std::domain_error,
                  "Value '" << value_ << "' is not supported!");
      const integer_enum_value_metadata & md = found->second;
      return md.has_group();
    }

    const std::string & parameter_model::get_enumerated_integer_value_group(const int value_) const
    {
      DT_THROW_IF(! is_integer(), std::logic_error, "Parameter model '" << get_name() << "' is not of integer type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
       const auto & found = _integer_enumeration_.find(value_);
      DT_THROW_IF(found == _integer_enumeration_.end(),
                  std::domain_error,
                  "Value '" << value_ << "' is not supported!");
      const integer_enum_value_metadata & md = found->second;
      DT_THROW_IF(!md.has_group(),
                  std::logic_error,
                  "Value '" << value_ << "' has no associated group!");
      return md.get_group();
    }

    void parameter_model::build_list_of_enumerated_integer_groups(std::set<std::string> & groups_) const
    {
      groups_.clear();
      for (integer_enum_dict_type::const_iterator i = _integer_enumeration_.begin();
           i != _integer_enumeration_.end();
           i++) {
        if (!i->second.has_group()) {
          groups_.insert(i->second.get_group());
        }
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_integer_values_in_group(const std::string & group_,
                                                                           std::set<int> & values_) const
    {
      values_.clear();
      for (integer_enum_dict_type::const_iterator i = _integer_enumeration_.begin();
           i != _integer_enumeration_.end();
           i++) {
        if (i->second.match_group(group_)) {
          values_.insert(i->first);
        }
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_integer_values(std::set<int> & values_) const
    {
      values_.clear();
      for (integer_enum_dict_type::const_iterator i = _integer_enumeration_.begin();
           i != _integer_enumeration_.end();
           i++) {
        values_.insert(i->first);
       }
      return;
    }

    bool parameter_model::has_enumerated_real_value(const double value_) const
    {
      bool has = false;
      if (! has_real_precision()) {
        has = _real_enumeration_.find(value_) != _real_enumeration_.end();
      } else {
        double prec = 0.5 * get_real_precision();
        for (real_enum_dict_type::const_iterator i = _real_enumeration_.begin();
             i != _real_enumeration_.end();
             i++) {
          double val = i->first;
          if ((value_ >= val - prec) && (value_ <= val + prec)) {
            has = true;
          }
        }
      }
      return has;
    }

    bool parameter_model::find_enumerated_real_value(const double value_, double & enum_value_) const
    {
      bool has = false;
      datatools::invalidate(enum_value_);
      double prec = 0.5 * get_real_precision();
      for (real_enum_dict_type::const_iterator i = _real_enumeration_.begin();
           i != _real_enumeration_.end();
           i++) {
        double val = i->first;
        if ((value_ >= val - prec) && (value_ <= val + prec)) {
          enum_value_ = val;
          has = true;
        }
      }
      return has;
    }

    const parameter_model::string_enum_dict_type & parameter_model::get_string_enumeration() const
    {
      return _string_enumeration_;
    }

    bool parameter_model::enumerated_real_value_has_group(const double value_) const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Parameter model '" << get_name() << "' is not of real type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Real parameter model '" << get_name() << "' does not use enumeration variable mode!");
      double value = datatools::invalid_real();
      for (const auto & re_pair : _real_enumeration_) {
        if (std::abs(value_ - re_pair.first) <= get_real_effective_precision()) {
          value = re_pair.first;
        }
      }
      DT_THROW_IF(!datatools::is_valid(value),
                  std::domain_error,
                  "Value '" << value_ << "' is not supported!");
      const auto & found = _real_enumeration_.find(value);
      const real_enum_value_metadata & md = found->second;
      return md.has_group();
    }

    const std::string & parameter_model::get_enumerated_real_value_group(const double value_) const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Parameter model '" << get_name() << "' is not of real type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Real parameter model '" << get_name() << "' does not use enumeration variable mode!");
      double value = datatools::invalid_real();
      for (const auto & re_pair : _real_enumeration_) {
        if (std::abs(value_ - re_pair.first) <= get_real_effective_precision()) {
          value = re_pair.first;
        }
      }
      DT_THROW_IF(!datatools::is_valid(value),
                  std::domain_error,
                  "Real parameter model '" << get_name() << "' has no enumerated value '" << value_ << "'!");
      const auto & found = _real_enumeration_.find(value);
      const real_enum_value_metadata & md = found->second;
      DT_THROW_IF(!md.has_group(),
                  std::logic_error,
                  "Value '" << value_ << "' has no associated group!");
      return md.get_group();
    }

    const parameter_model::real_enum_value_metadata &
    parameter_model::get_enumerated_real_value_metadata(const double value_) const
    {
      DT_THROW_IF(! is_real(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of real type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error,
                  "Real parameter model '" << get_name() << "' does not use enumeration variable mode!");
      double value = datatools::invalid_real();
      for (const auto & re_pair : _real_enumeration_) {
        if (std::abs(value_ - re_pair.first) <= get_real_effective_precision()) {
          value = re_pair.first;
        }
      }
      DT_THROW_IF(!datatools::is_valid(value),
                  std::domain_error,
                  "Real parameter model '" << get_name() << "' has no enumerated value '" << value_ << "'!");
      const auto & found = _real_enumeration_.find(value);
      return found->second;
    }

    parameter_model::real_enum_value_metadata &
    parameter_model::grab_enumerated_real_value_metadata(const double value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      const auto & const_this = const_cast<parameter_model*>(this);
      const auto & md = const_this->get_enumerated_real_value_metadata(value_);
      return const_cast<parameter_model::real_enum_value_metadata &>(md);
    }

    void parameter_model::add_enumerated_real_value(const double value_,
                                                    const bool default_)
    {
      real_enum_value_metadata null;
      add_enumerated_real_value(value_, null, default_);
      return;
    }

    void parameter_model::add_enumerated_real_value(const double value_,
                                                    const real_enum_value_metadata & metadata_,
                                                    const bool default_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_real(), std::logic_error, "Parameter model '" << get_name() << "' is not of real type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
      DT_THROW_IF(has_enumerated_real_value(value_),
                  std::logic_error,
                  "Real parameter model '" << get_name() << "' "
                  << "already has enumeration value '" << value_ << "' !");
      _real_enumeration_[value_] = metadata_;
      if (default_) {
        set_default_real(value_);
      }
      return;
    }

    void parameter_model::fetch_real_enumeration(std::vector<double> & renums_) const
    {
      DT_THROW_IF(! is_real(), std::logic_error, "Parameter model '" << get_name() << "' is not of real type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
      renums_.clear();
      for (real_enum_dict_type::const_iterator i = _real_enumeration_.begin();
           i != _real_enumeration_.end();
           i++) {
        renums_.push_back(i->first);
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_real_groups(std::set<std::string> & groups_) const
    {
      groups_.clear();
      for (real_enum_dict_type::const_iterator i = _real_enumeration_.begin();
           i != _real_enumeration_.end();
           i++) {
        if (i->second.has_group()) {
          groups_.insert(i->second.get_group());
        }
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_real_values_in_group(const std::string & group_,
                                                                        std::set<double> & values_) const
    {
      values_.clear();
      for (real_enum_dict_type::const_iterator i = _real_enumeration_.begin();
           i != _real_enumeration_.end();
           i++) {
        if (i->second.match_group(group_)) {
          values_.insert(i->first);
        }
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_real_values(std::set<double> & values_) const
    {
      values_.clear();
      for (real_enum_dict_type::const_iterator i = _real_enumeration_.begin();
           i != _real_enumeration_.end();
           i++) {
        values_.insert(i->first);
       }
      return;
    }

    bool parameter_model::has_enumerated_string_value(const std::string & value_) const
    {
      return _string_enumeration_.find(value_) != _string_enumeration_.end();
    }

    bool parameter_model::enumerated_string_value_has_group(const std::string & value_) const
    {
      const auto & found = _string_enumeration_.find(value_);
      DT_THROW_IF(found == _string_enumeration_.end(),
                  std::domain_error,
                  "Value '" << value_ << "' is not supported!");
      const string_enum_value_metadata & md = found->second;
      return md.has_group();
    }

    const std::string & parameter_model::get_enumerated_string_value_group(const std::string & value_) const
    {
      const auto & found = _string_enumeration_.find(value_);
      DT_THROW_IF(found == _string_enumeration_.end(),
                  std::domain_error,
                  "Value '" << value_ << "' is not supported!");
      const string_enum_value_metadata & md = found->second;
      DT_THROW_IF(!md.has_group(),
                  std::logic_error,
                  "Value '" << value_ << "' has no associated group!");
      return md.get_group();
    }

    void parameter_model::add_enumerated_string_value(const std::string & value_,
                                                      const bool default_)
    {
      string_enum_value_metadata null;
      add_enumerated_string_value(value_, null, default_);
      return;
    }

    void parameter_model::add_enumerated_string_value(const std::string & value_,
                                                      const string_enum_value_metadata & metadata_,
                                                      const bool default_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_string(), std::logic_error, "Parameter model '" << get_name() << "' is not of string type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "String parameter model '" << get_name() << "' does not use enumeration variable mode!");
      DT_THROW_IF(has_enumerated_string_value(value_),
                  std::logic_error,
                  "String parameter model '" << get_name() << "' "
                  << "already has enumeration value '" << value_ << "' !");
      _string_enumeration_[value_] = metadata_;
      if (default_) {
        set_default_string(value_);
      }
      return;
    }

    const parameter_model::string_enum_value_metadata &
    parameter_model::get_enumerated_string_value_metadata(const std::string & value_) const
    {
      DT_THROW_IF(! is_string(), std::logic_error, "Parameter model '" << get_name() << "' is not of string type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "String parameter model '" << get_name() << "' does not use enumeration variable mode!");
      string_enum_dict_type::const_iterator found = _string_enumeration_.find(value_);
      DT_THROW_IF(found == _string_enumeration_.end(),
                  std::logic_error,
                  "String parameter model '" << get_name() << "' has no enumerated value '" << value_ << "'!");
      return found->second;
    }

    parameter_model::string_enum_value_metadata &
    parameter_model::grab_enumerated_string_value_metadata(const std::string & value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_string(), std::logic_error, "Parameter model '" << get_name() << "' is not of string type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "String parameter model '" << get_name() << "' does not use enumeration variable mode!");
      string_enum_dict_type::iterator found = _string_enumeration_.find(value_);
      DT_THROW_IF(found == _string_enumeration_.end(),
                  std::logic_error,
                  "String parameter model '" << get_name() << "' has no enumerated value '" << value_ << "'!");
      return found->second;
    }

    void parameter_model::build_list_of_enumerated_string_groups(std::set<std::string> & groups_) const
    {
      groups_.clear();
      for (string_enum_dict_type::const_iterator i = _string_enumeration_.begin();
           i != _string_enumeration_.end();
           i++) {
        if (i->second.has_group()) {
          groups_.insert(i->second.get_group());
        }
      }
      return;
    }

    struct compare_enum_string_values_by_rank
    {
      compare_enum_string_values_by_rank(const parameter_model::string_enum_dict_type & str_enum_)
        : str_enum(str_enum_) {}
      bool operator()(const std::string & value1_, const std::string & value2_) const
      {
        return parameter_model::compare_enum_value_metadata_by_rank(str_enum.find(value1_)->second, str_enum.find(value2_)->second);
      } 
      const parameter_model::string_enum_dict_type & str_enum;
    };

    void parameter_model::rank_list_of_enumerated_string_values(const std::set<std::string> & values_,
                                                                std::list<std::string> & ranked_values_) const
    {
      ranked_values_.clear();
      for (const auto & value : values_) {
        ranked_values_.push_back(value);
      }
      compare_enum_string_values_by_rank compareByRank(_string_enumeration_);
      ranked_values_.sort(compareByRank);
      return;
    }
  
    void parameter_model::build_list_of_enumerated_string_values_in_group(const std::string & group_,
                                                                          std::set<std::string> & values_) const
    {
      values_.clear();
      for (string_enum_dict_type::const_iterator i = _string_enumeration_.begin();
           i != _string_enumeration_.end();
           i++) {
        if (i->second.match_group(group_)) {
          values_.insert(i->first);
        }
      }
      return;
    }

    void parameter_model::build_list_of_enumerated_string_values(std::set<std::string> & values_) const
    {
      DT_THROW_IF(! is_string(), std::logic_error, "Parameter model '" << get_name() << "' is not of string type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "String parameter model '" << get_name() << "' does not use enumeration variable mode!");
      values_.clear();
      for (string_enum_dict_type::const_iterator i = _string_enumeration_.begin();
           i != _string_enumeration_.end();
           i++) {
        values_.insert(i->first);
       }
      return;
    }

    //
    // Groups
    //

    bool parameter_model::has_groups() const
    {
      return _groups_.size() > 0;
    }

    bool parameter_model::has_group(const std::string & group_name_) const
    {
      return _groups_.count(group_name_);
    }

    void parameter_model::add_group(const std::string & group_name_,
                                    const std::string & doc_)
    {
      if (_groups_.count(group_name_) == 1) return;
      DT_THROW_IF(!::datatools::configuration::validate_group_name(group_name_),
                  std::domain_error,
                  "Invalid group name '" << group_name_ << "'!");
      _groups_[group_name_] = doc_;
      return;
    }

    void parameter_model::build_set_of_groups(std::set<std::string> & groups_) const
    {
      groups_.clear();
      for (const auto & group_pair : _groups_) {
        groups_.insert(group_pair.first);
      }
      return;
    }

    //
    // Variants:
    //

    bool parameter_model::has_variant(const std::string & variant_name_) const
    {
      variant_dict_type::const_iterator found_variant = _variants_.find(variant_name_);
      return found_variant != _variants_.end();
    }

    void parameter_model::add_variant(const std::string & variant_name_,
                                      const vm_handle_type & variant_model_handle_,
                                      const std::string & description_ )
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Configuration parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(!validate_instance_name(variant_name_), std::logic_error,
                  "Variant name '" << variant_name_ << "' is not allowed "
                  << "in configuration parameter model '" << get_name() << "'!");
      DT_THROW_IF(!variant_model_handle_, std::logic_error,
                  "Not a valid variant model for variant named '" << variant_name_ << "' "
                  << "in configuration parameter model '" << get_name() << "' !");
      variant_dict_type::const_iterator found_variant = _variants_.find(variant_name_);
      DT_THROW_IF(found_variant != _variants_.end(), std::logic_error,
                  "Configuration parameter model '" << get_name() << "' already has a variant labelled '"
                  << variant_name_ << "'!");
      {
        variant_physical dummy;
        _variants_[variant_name_] = dummy;
      }
      variant_physical & ve = _variants_.find(variant_name_)->second;
      ve.set(variant_name_, description_, variant_model_handle_);
      return;
    }

    void parameter_model::remove_variant(const std::string & variant_name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Configuration parameter model '" << get_name() << "' is already locked !");
      variant_dict_type::const_iterator found_variant = _variants_.find(variant_name_);
      DT_THROW_IF(found_variant != _variants_.end(), std::logic_error,
                  "Configuration parameter model '" << get_name() << "' "
                  << "does not have a variant named '" << variant_name_ << "' !");

      if (is_boolean()) {
        if (_false_metadata_.has_variant(variant_name_)) {
          _false_metadata_.remove_variant(variant_name_);
        }
        if (_true_metadata_.has_variant(variant_name_)) {
          _true_metadata_.remove_variant(variant_name_);
        }
      }

      if (is_integer()) {
        if (is_enumeration()) {
          for (integer_enum_dict_type::iterator i = _integer_enumeration_.begin();
               i != _integer_enumeration_.end();
               i++) {
            if ( i->second.has_variant(variant_name_)) {
              i->second.remove_variant(variant_name_);
            }
          }
        }
        if (is_interval()) {
          for (integer_range_enum_dict_type::iterator i = _integer_ranges_.begin();
               i != _integer_ranges_.end();
               i++) {
            if ( i->second.has_variant(variant_name_)) {
               i->second.remove_variant(variant_name_);
            }
          }
        }
      }

      if (is_real()) {
        if (is_enumeration()) {
          for (real_enum_dict_type::iterator i = _real_enumeration_.begin();
               i != _real_enumeration_.end();
               i++) {
            if ( i->second.has_variant(variant_name_)) {
              i->second.remove_variant(variant_name_);
            }
          }
        }
        if (is_interval()) {
          for (real_range_enum_dict_type::iterator i = _real_ranges_.begin();
               i != _real_ranges_.end();
               i++) {
            if ( i->second.has_variant(variant_name_)) {
              i->second.remove_variant(variant_name_);
            }
          }
        }
      }

      if (is_string()) {
        if (is_enumeration()) {
          for (string_enum_dict_type::iterator i = _string_enumeration_.begin();
               i != _string_enumeration_.end();
               i++) {
            if ( i->second.has_variant(variant_name_)) {
              i->second.remove_variant(variant_name_);
             }
          }
        }
      }

      _variants_.erase(variant_name_);

      return;
    }

    const std::string & parameter_model::get_variant_description(const std::string & variant_name_) const
    {
      variant_dict_type::const_iterator found_variant = _variants_.find(variant_name_);
      DT_THROW_IF(found_variant == _variants_.end(),
                  std::logic_error,
                  "Configuration parameter model '" << get_name() << "' has no variant named '"
                  << variant_name_ << "' !");
      return found_variant->second.get_terse_description();
    }

    const variant_model & parameter_model::get_variant_model(const std::string & variant_name_) const
    {
      variant_dict_type::const_iterator found_variant = _variants_.find(variant_name_);
      DT_THROW_IF(found_variant == _variants_.end(),
                  std::logic_error,
                  "Configuration parameter model '" << get_name() << "' has no variant named '"
                  << variant_name_ << "' !");
      return found_variant->second.get_model();
    }

    vm_handle_type parameter_model::get_variant_model_handle(const std::string & variant_name_) const
    {
      variant_dict_type::const_iterator found_variant = _variants_.find(variant_name_);
      DT_THROW_IF(found_variant == _variants_.end(),
                  std::logic_error,
                  "Parameter model '" << get_name() << "' has no variant named '"
                  << variant_name_ << "' !");
      return found_variant->second.get_model_handle();
    }

    void parameter_model::associate_variant_to_boolean(const bool value_, const std::string & variant_name_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_boolean(),
                  std::logic_error,
                  "Parameter model '" << get_name() << "' is not of boolean type!");
      DT_THROW_IF(! has_variant(variant_name_),
                  std::logic_error,
                  "Boolean parameter model '" << get_name() << "' "
                  << "has no variant labelled '" << variant_name_ << "' !");
      if (value_) {
        _true_metadata_.add_variant(variant_name_);
      } else {
        _false_metadata_.add_variant(variant_name_);
      }
      return;
    }

    void parameter_model::associate_variant_to_integer(const int value_, const std::string & variant_name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_integer(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of integer type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error,
                  "Integer parameter model '" << get_name() << "' does not use enumeration variable mode!");
      DT_THROW_IF(! has_enumerated_integer_value(value_),
                  std::logic_error,
                  "Integer parameter model '" << get_name() << "' "
                  << "has no enumeration value '" << value_ << "' !");
      DT_THROW_IF(! has_variant(variant_name_),
                  std::logic_error,
                  "Integer parameter model '" << get_name() << "' "
                  << "has no variant labelled '" << variant_name_ << "' !");
      integer_enum_dict_type::iterator found =
        _integer_enumeration_.find(value_);
      found->second.add_variant(variant_name_);
      return;
    }

    void parameter_model::add_integer_range(const integer_range & ir_,
                                            const integer_range_enum_metadata & ir_data_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_integer(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of integer type!");
      if (is_enumeration()) {
        // We should check that the given range contains at least one enumerated value...
        // Not implemented yet!
      }
      _integer_ranges_[ir_] = ir_data_;
      if (ir_data_.has_group()) {
        add_group(ir_data_.get_group());
      }
      return;
    }

    void parameter_model::associate_variant_to_integer_range(const integer_range & ir_,
                                                             const std::string & variant_name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_integer(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of integer type!");
      // In principle, this kind of association should work in free, enumeration and interval mutability mode:
      // So we do not force the parameter to be in interval mode only...
      if (is_interval()) {
        // In interval mode, the given range must be contained in the domain:
        DT_THROW_IF(! _integer_domain_.has(ir_),
                    std::logic_error,
                    "Integer parameter model '" << get_name() << "' "
                    << "domain does not contain range '" << ir_ << "' !");
      }
      DT_THROW_IF(! has_variant(variant_name_),
                  std::logic_error,
                  "Integer parameter model '" << get_name() << "' "
                  << "has no variant labelled '" << variant_name_ << "' !");
      integer_range_enum_dict_type::iterator found_ir = _integer_ranges_.find(ir_);
      if (found_ir == _integer_ranges_.end()) {
        integer_range_enum_metadata ie_data;
        ie_data.add_variant(variant_name_);
        add_integer_range(ir_,ie_data);
      } else {
        found_ir->second.add_variant(variant_name_);
      }
      return;
    }

    void parameter_model::add_real_range(const real_range & rr_,
                                         const real_range_enum_metadata & rr_data_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_real(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of real type!");
      if (is_enumeration()) {
        // We should check that the given range contains at least one enumerated value...
        // Not implemented yet!
      }
      _real_ranges_[rr_] = rr_data_;
      if (rr_data_.has_group()) {
        add_group(rr_data_.get_group());
      }
      return;
    }

    void parameter_model::associate_variant_to_real(const double value_, const std::string & variant_name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Description of parameter_model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_real(), std::logic_error, "Parameter_Model '" << get_name() << "' is not of real type!");
      DT_THROW_IF(! is_enumeration(), std::logic_error, "Real parameter_model '" << get_name() << "' does not use enumeration variable_mode!");
      double enum_value;
      bool found = find_enumerated_real_value(value_, enum_value);
      DT_THROW_IF(! found,
                  std::logic_error,
                  "Real parameter_model '" << get_name() << "' "
                  << "has no enumeration value '" << value_ << "' !");
      DT_THROW_IF(! has_variant(variant_name_),
                  std::logic_error,
                  "Real parameter_model '" << get_name() << "' "
                  << "has no variant labelled '" << variant_name_ << "' !");
      real_enum_dict_type::iterator real_found =
        _real_enumeration_.find(enum_value);
      real_found->second.add_variant(variant_name_);
      return;
    }

    void parameter_model::associate_variant_to_real_range(const real_range & rr_,
                                                          const std::string & variant_name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_real(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of real type!");
      // In principle, this kind of association should work in free, enumeration and interval mutability mode:
      // So we do not force the parameter to be in interval mode only...
      if (is_interval()) {
        // In interval mode, the given range must be contained in the domain:
        DT_THROW_IF(! _real_domain_.has(rr_),
                    std::logic_error,
                    "Real parameter model '" << get_name() << "' "
                    << "domain does not contain range '" << rr_ << "' !");
      }
      DT_THROW_IF(! has_variant(variant_name_),
                  std::logic_error,
                  "Real parameter model '" << get_name() << "' "
                  << "has no variant labelled '" << variant_name_ << "' !");
      real_range_enum_dict_type::iterator found_rr = _real_ranges_.find(rr_);
      if (found_rr == _real_ranges_.end()) {
        real_range_enum_metadata re_data;
        re_data.add_variant(variant_name_);
        add_real_range(rr_,re_data);
      } else {
        found_rr->second.add_variant(variant_name_);
      }
      return;
    }

    void parameter_model::associate_variant_to_string(const std::string & value_,
                                                      const std::string & variant_name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Description of parameter model '" << get_name() << "' is locked!");
      DT_THROW_IF(! is_string(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not of string type!");
      DT_THROW_IF(! is_enumeration() && ! is_free(), std::logic_error,
                  "String parameter model '" << get_name() << "' does not use enumeration or free variable mode!");
      DT_THROW_IF(! has_enumerated_string_value(value_),
                  std::logic_error,
                  "String parameter model '" << get_name() << "' "
                  << "has no enumeration value '" << value_ << "' !");
      DT_THROW_IF(! has_variant(variant_name_),
                  std::logic_error,
                  "String parameter model '" << get_name() << "' "
                  << "has no variant labelled '" << variant_name_ << "' !");
      string_enum_dict_type::iterator found = _string_enumeration_.find(value_);
      found->second.add_variant(variant_name_);
      return;
    }

    const parameter_model::integer_range_enum_dict_type &
    parameter_model::get_integer_ranges() const
    {
      return _integer_ranges_;
    }

    const parameter_model::real_range_enum_dict_type &
    parameter_model::get_real_ranges() const
    {
      return _real_ranges_;
    }

    bool parameter_model::is_initialized() const
    {
      return _initialized_;
    }

    void parameter_model::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is not initialized!");
      _initialized_ = false;
      _set_default();
      this->enriched_base::clear();
      return;
    }

    void parameter_model::initialize(const properties & setup_, model_item_dict_type & items_)
    {
      // std::cerr << "===> DEVEL: parameter_model::initialize: "
      //           << "Initializing parameter model '" << get_name() << "'...\n";
      // setup_.tree_dump(std::cerr, "Setup: ", "===> DEVEL: ");

      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Parameter model '" << get_name() << "' is already initialized!");
      enriched_base::initialize(setup_);

      DT_LOG_TRACE(get_logging_priority(),
                   "Initializing parameter model '" << get_name() << "'...");

      // Parse type if needed:
      if (!has_type()) {
        basic_type type = TYPE_NONE;
        DT_THROW_IF(!setup_.has_key("type"), std::logic_error, "Missing 'type' property !");
        const std::string & type_label = setup_.fetch_string("type");
        type = (basic_type) get_type_from_label(type_label);
        DT_THROW_IF(type == TYPE_NONE, std::logic_error, "Unsupported type '" << type_label << "' !");
        DT_THROW_IF(type == TYPE_NONE, std::logic_error, "Undefined type !");
        // Set type:
        set_type(type);
      } // type

      // Mandatory type:
      DT_THROW_IF(!has_type(), std::logic_error, "Undefined type !");

      // Special string case: path
      if (is_string()) {

        if (setup_.has_flag("path")) {
          set_path(true);
        }

        if (is_path()) {
          if (! has_path_io()) {
            if (setup_.has_key("path.io")) {
              std::string path_io_label = setup_.fetch_string("path.io");
              if (path_io_label == label_from_path_io(PATH_INPUT)) {
                set_path_io(PATH_INPUT);
              } else if (path_io_label == label_from_path_io(PATH_OUTPUT)) {
                set_path_io(PATH_OUTPUT);
              } else if (path_io_label == label_from_path_io(PATH_INPUT_OUTPUT)) {
                set_path_io(PATH_INPUT_OUTPUT);
              } else {
                DT_THROW_IF(true, std::logic_error,
                            "Undefined path I/O mode '" << path_io_label << "'!");
              }
            }
          }
          if (! has_path_io()) {
            // Default:
            set_path_io(PATH_INPUT);
          }
        }

      }

      // Parse mutability if needed:
      if (!has_mutability()) {
        mutability_type mutability = MUTABILITY_UNDEFINED;
        if (setup_.has_key("mutability")) {
          const std::string & mutability_label = setup_.fetch_string("mutability");
          mutability = (mutability_type) mutability_from_label(mutability_label);
          DT_THROW_IF(mutability == MUTABILITY_UNDEFINED, std::logic_error,
                      "Unsupported mutability '" << mutability_label << "' !");
        }
        if (mutability == MUTABILITY_UNDEFINED) {
          mutability = MUTABILITY_DEFAULT;
        }
        set_mutability(mutability);
      } // mutability
      // Mandatory mutability:
      DT_THROW_IF(!has_mutability(),std::logic_error,
                  "Parameter model '" <<  get_name() << "' has no mutability!");

      // Parse real unit:
      std::string real_unit_label;
      std::string real_preferred_unit_symbol;
      double real_default_unit = 1.0;
      if (is_real()) {

        if (setup_.has_key("real.unit_label")) {
          real_unit_label = setup_.fetch_string("real.unit_label");
          set_real_unit_label(real_unit_label);
        }

        if (setup_.has_key("real.preferred_unit")) {
          real_preferred_unit_symbol = setup_.fetch_string("real.preferred_unit");
          set_real_preferred_unit(real_preferred_unit_symbol);
        }

        fetch_real_display_unit(real_preferred_unit_symbol, real_default_unit);
        /*
        if (real_preferred_unit_symbol.empty() && ! real_unit_label.empty()) {
          real_preferred_unit_symbol = units::get_default_unit_symbol_from_label(real_unit_label);
        }
        if (! real_preferred_unit_symbol.empty()) {
          real_default_unit = units::get_unit(real_preferred_unit_symbol);
          set_real_preferred_unit(real_preferred_unit_symbol);
        }
        */
        if (! has_real_precision()) {
          double real_precision = std::numeric_limits<int>::quiet_NaN();
          if (setup_.has_key("real.precision")) {
            real_precision = setup_.fetch_real("real.precision");
            if (! setup_.has_explicit_unit("real.precision")) {
              real_precision *= real_default_unit;
            }
          }
          set_real_precision(real_precision);
        }
        // Mandatory real precision:
        DT_THROW_IF (! has_real_precision(),
                     std::logic_error,
                     "Real precision is not defined for parameter model '" <<  get_name() << "' !");
      } // real

      if (is_variable()) {

        if (! has_variable_mode()) {
          // Parse variable_mode:
          variable_mode_type variable_mode = VARIABLE_MODE_UNDEFINED;
          if (setup_.has_key("variable.mode")) {
            const std::string & variable_mode_label = setup_.fetch_string("variable.mode");
            if (variable_mode_label == label_from_variable_mode(VARIABLE_MODE_FREE)) {
              variable_mode = VARIABLE_MODE_FREE;
            } else if (variable_mode_label == label_from_variable_mode(VARIABLE_MODE_ENUM)) {
              variable_mode = VARIABLE_MODE_ENUM;
            } else if (variable_mode_label == label_from_variable_mode(VARIABLE_MODE_INTERVAL)) {
              variable_mode = VARIABLE_MODE_INTERVAL;
            } else {
              DT_THROW_IF(true, std::logic_error, "Unsupported variable mode '" << variable_mode_label << "' !");
            }
          }
          if (variable_mode == VARIABLE_MODE_UNDEFINED) {
            variable_mode = VARIABLE_MODE_DEFAULT;
          }
          set_variable_mode(variable_mode);
        }
        // Mandatory variable mode:
        DT_THROW_IF(!has_variable_mode(),std::logic_error,
                    "Parameter model '" <<  get_name() << "' has no variable mode!");

        // Groups:
        if (setup_.has_key("groups")) {
          std::set<std::string> group_names;
          setup_.fetch("groups", group_names);
          for (const auto & group_name : group_names) {
            std::string group_doc;
            {
              // Parse group documentation:
              std::ostringstream group_doc_ss;
              group_doc_ss << "groups." << group_name << ".documentation";
              if (setup_.has_key(group_doc_ss.str())) {
                group_doc = setup_.fetch_string(group_doc_ss.str());
              }
            }
            add_group(group_name, group_doc);
          }
        }

        // Referenced variants:
        /* Example with 3 embedded variants:
         *
         *  variants : string[2] = "LE" "IE" "HE"
         *  variants.LE.model       : string = "low_energy_processes.VM"
         *  variants.LE.description : string = "Variant model dedicated to low energy processes"
         *  variants.IE.model       : string = "medium_energy_processes.VM"
         *  variants.IE.description : string = "Variant model dedicated to intermediate energy processes"
         *  variants.HE.model       : string = "high_energy_processes.VM"
         *  variants.HE.description : string = "Variant model dedicated to high energy processes"
         *
         */
        if (setup_.has_key("variants")) {
          std::vector<std::string> variant_names;
          setup_.fetch("variants", variant_names);
          for (int i = 0; i < (int) variant_names.size(); i++) {
            const std::string & var_name = variant_names[i];
            DT_THROW_IF(has_variant(var_name), std::logic_error,
                        "Variant '" << var_name << "' is already used!");
            std::ostringstream var_model_ss;
            var_model_ss << "variants." << var_name << ".model";
            DT_THROW_IF(!setup_.has_key(var_model_ss.str()), std::logic_error,
                        "Property '" << var_model_ss.str() << "' is missing!");
            const std::string & var_model = setup_.fetch_string(var_model_ss.str());
            model_item_dict_type::const_iterator found = items_.find(var_model);
            DT_THROW_IF(found == items_.end(), std::logic_error,
                        "Item '" << var_model << "' does not exist!");
            const model_item & var_item = found->second;
            DT_THROW_IF(! var_item.is_variant(), std::logic_error,
                        "Item '" << var_model << "' is not a variant!");
            std::ostringstream var_desc_ss;
            var_desc_ss << "variants." << var_name << ".description";
            std::string var_desc;
            if (setup_.has_key(var_desc_ss.str())) {
              var_desc = setup_.fetch_string(var_desc_ss.str());
            }
            add_variant(var_name, var_item.get_variant_handle(), var_desc);
          }
        }

        // Boolean
        if (is_boolean()) {
          // Variants associated to boolean values:
          {
            datatools::properties true_setup;
            setup_.export_and_rename_starting_with(true_setup, "boolean.true.", "");
            if (datatools::logger::is_trace(get_logging_priority())) {
              true_setup.tree_dump(std::clog, "True setup:", "[trace]: ");
            }
            // DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Parameter model = '" << get_name() << "' : ");
            boolean_enum_value_metadata true_metadata;
            true_metadata.initialize(true_setup, *this);
            set_enumerated_boolean_value_metadata(true, true_metadata);
          }
          {
            datatools::properties false_setup;
            setup_.export_and_rename_starting_with(false_setup, "boolean.false.", "");
            if (datatools::logger::is_trace(get_logging_priority())) {
              false_setup.tree_dump(std::clog, "False setup:", "[trace]: ");
            }
            boolean_enum_value_metadata false_metadata;
            false_metadata.initialize(false_setup, *this);
            set_enumerated_boolean_value_metadata(false, false_metadata);
          }
        }

        // Parse interval parameter_models:
        if (is_interval()) {

          if (is_integer()) {

            if (! _integer_domain_.is_valid()) {
              /* Example:
               *
               *  integer.domain : string = "[1,10]"
               *
               */
              if (setup_.has_key("integer.domain")) {
                std::string iid_str = setup_.fetch_string("integer.domain");
                std::istringstream iid_iss(iid_str);
                integer_range ir;
                iid_iss >> ir;
                DT_THROW_IF(!iid_iss, std::logic_error,
                            "Invalid format for integer range from '" << iid_str << "'!");
                set_integer_domain(ir);
              } else {
                DT_THROW_IF(true, std::logic_error,
                            "Missing '" << "interval.integer_domain" << "' property!");
              }
            }

            // Parse integer range-to-variant associations:
            /* Example:
             *
             *  integer.domain : string = "[1,10]"
             *  integer.ranges : string[3] = "small" "medium" " large"
             *
             *  integer.ranges.small.range    : string = "[1,4)"
             *  integer.ranges.small.documentation : string = "Domain of small values"
             *  integer.ranges.small.variant  : string = "small_regime"
             *
             *  integer.ranges.medium.range   : string = "[4,8)"
             *  integer.ranges.medium.documentation : string = "Domain of intermediate values"
             *  integer.ranges.medium.variant : string = "medium_regime"
             *
             *  integer.ranges.large.range    : string = "[8,10]"
             *  integer.ranges.large.documentation : string = "Domain of large values"
             *  integer.ranges.large.variant  : string = "large_regime"
             *
             */
            if (setup_.has_key("integer.ranges")) {
              std::vector<std::string> ii_ranges_labels;
              setup_.fetch("integer.ranges", ii_ranges_labels);
              for (size_t i = 0; i < ii_ranges_labels.size(); i++) {
                std::string ii_ranges_label = ii_ranges_labels[i];
                integer_range ir;
                std::ostringstream ii_prefix_ss;
                ii_prefix_ss << "integer.ranges." << ii_ranges_label << ".";

                std::ostringstream ii_r_ss;
                ii_r_ss << ii_prefix_ss.str() << "range";
                {
                  DT_THROW_IF(!setup_.has_key(ii_r_ss.str()),
                              std::logic_error,
                              "Missing key '" << ii_r_ss.str()<< "' (integer range) !");
                  std::string ii_r_str = setup_.fetch_string(ii_r_ss.str());
                  std::istringstream ir_iss(ii_r_str);
                  ir_iss >> ir >> std::ws;
                  DT_THROW_IF(!ir_iss, std::logic_error,
                              "Invalid integer range format for '" << ii_r_str << "' !");
                }
                datatools::properties ir_setup;
                setup_.export_and_rename_starting_with(ir_setup, ii_prefix_ss.str(), "");
                integer_range_enum_metadata ir_entry;
                ir_entry.initialize(ir_setup, *this);
                add_integer_range(ir, ir_entry);

              } // for integer range-to-variant associations

            } // integer range-to-variant associations

          } // variable/interval/integer

          if (is_real()) {

            if (! _real_domain_.is_valid()) {
              /* Example:
               *
               *  real.domain : string = "[10 um,)"
               *
               */
              if (setup_.has_key("real.domain")) {
                std::string ird_str = setup_.fetch_string("real.domain");
                std::istringstream ird_iss(ird_str);
                real_range rr;
                ird_iss >> rr;
                DT_THROW_IF(!ird_iss, std::logic_error,
                            "Invalid format for real range from '" << ird_str << "'!");
                set_real_domain(rr);
              } else {
                DT_THROW_IF(true, std::logic_error, "Missing '" << "interval.integer_domain" << "' property!");
              }
            }

            // Parse integer range-to-variant associations:
            /* Example:
             *
             *  real.domain : string = "[10 um,)"
             *  real.ranges : string[3] = "small" "medium" " large"
             *  real.ranges.small.range    : string = "[10 um,30um)"
             *  real.ranges.small.variant  : string = "small_regime"
             *  real.ranges.medium.range   : string = "[30 um, 200 um)"
             *  real.ranges.medium.variant : string = "medium_regime"
             *  real.ranges.large.range    : string = "[200um,)"
             *  real.ranges.large.variant  : string = "large_regime"
             *
             */
            if (setup_.has_key("real.ranges")) {
              std::vector<std::string> ri_ranges_labels;
              setup_.fetch("real.ranges", ri_ranges_labels);
              for (int i = 0; i < (int) ri_ranges_labels.size(); i++) {
                std::string ri_ranges_label = ri_ranges_labels[i];
                real_range rr;
                std::ostringstream ri_prefix_ss;
                ri_prefix_ss << "real.ranges." << ri_ranges_label << ".";
                std::ostringstream ri_r_ss;
                ri_r_ss << ri_prefix_ss.str() << "range";
                {
                  DT_THROW_IF(!setup_.has_key(ri_r_ss.str()),
                              std::logic_error,
                              "Missing key '" << ri_r_ss.str()<< "' (integer range) !");
                  std::string ri_r_str = setup_.fetch_string(ri_r_ss.str());
                  std::istringstream rr_iss(ri_r_str);
                  rr_iss >> rr >> std::ws;
                  DT_THROW_IF(!rr_iss, std::logic_error,
                              "Invalid real range format for '" << ri_r_str << "' !");
                }
                datatools::properties rr_setup;
                setup_.export_and_rename_starting_with(rr_setup, ri_prefix_ss.str(), "");
                real_range_enum_metadata rr_entry;
                rr_entry.initialize(rr_setup, *this);
                add_real_range(rr, rr_entry);
              } // for real range-to-variant associations

            } // real range-to-variant associations

          } // variable/interval/real

        } // variable/interval

        if (is_enumeration()) {

          if (is_integer()) {
            /* Example:
             *
             *   integer.enumerated.size      : integer = 4
             *   integer.enumerated_0.value   : integer = 1
             *   integer.enumerated_0.group   : string  = "Small"
             *   integer.enumerated_0.default : boolean = true"
             *   integer.enumerated_0.variant : string  = "regime.none"
             *   integer.enumerated_1.value   : integer = 2
             *   integer.enumerated_1.variant : string  = "regime.single"
             *   integer.enumerated_2.value   : integer = 4
             *   integer.enumerated_2.variant : string  = "regime.several"
             *   integer.enumerated_3.value   : integer = 8
             *   integer.enumerated_3.variant : string  = "regime.several"
             *
             */
            if (_integer_enumeration_.size() == 0) {
              bool default_is_set = false;
              int number_of_enumerated = 0;
              if (setup_.has_key("integer.enumerated.size")) {
                number_of_enumerated = setup_.fetch_positive_integer("integer.enumerated.size");
              }
              for (int i = 0; i < number_of_enumerated; i++) {
                bool this_as_default = false;
                std::ostringstream ie_prefix_ss;
                ie_prefix_ss <<  "integer.enumerated_" <<  i << ".";
                std::ostringstream ie_value_ss;
                ie_value_ss << ie_prefix_ss.str() << "value";
                DT_THROW_IF(!setup_.has_key(ie_value_ss.str()),
                            std::logic_error,
                            "Missing key '" << ie_value_ss.str()<< "' (integer enumerated value #" << i << ") !");
                int ie_value = setup_.fetch_integer(ie_value_ss.str());

                datatools::properties ie_setup;
                setup_.export_and_rename_starting_with(ie_setup, ie_prefix_ss.str(), "");
                integer_enum_value_metadata int_ev_metadata;
                int_ev_metadata.initialize(ie_setup, *this);

                {
                  std::ostringstream ie_default_by_ss;
                  ie_default_by_ss << ie_prefix_ss.str() << "default";
                  if (setup_.has_key(ie_default_by_ss.str())) {
                    DT_THROW_IF(default_is_set, std::logic_error,
                                "Default enumerated integer value is already set!");
                    bool def = setup_.fetch_boolean(ie_default_by_ss.str());
                    if (def) {
                      default_is_set = true;
                      this_as_default = true;
                    }
                  }
                }
                add_enumerated_integer_value(ie_value, int_ev_metadata, this_as_default);
              }
            }
            DT_THROW_IF(_integer_enumeration_.size() == 0, std::logic_error,
                        "Missing explicit supported enumeration of integer values!");
          } // variable/enumeration/integer

          if (is_real()) {
            /* Example:
             *
             *   real.enumerated.size      : integer = 3
             *   real.enumerated_0.value   : real = 1.0 s
             *   real.enumerated_0.variant : string  = "regime.fast"
             *   real.enumerated_1.value   : real = 10.0 s
             *   real.enumerated_1.variant : string  = "regime.normal"
             *   real.enumerated_2.value   : real = 100.0 s
             *   real.enumerated_2.variant : string  = "regime.slow"
             *
             */
            if (_real_enumeration_.size() == 0) {
              bool default_is_set = false;
              int number_of_enumerated = 0;
              if (setup_.has_key("real.enumerated.size")) {
                number_of_enumerated = setup_.fetch_positive_integer("real.enumerated.size");
              }
              for (int i = 0; i < number_of_enumerated; i++) {
                bool this_as_default = false;
                std::ostringstream re_prefix_ss;
                re_prefix_ss <<  "real.enumerated_" <<  i << ".";
                std::ostringstream re_value_ss;
                re_value_ss << re_prefix_ss.str() << "value";
                DT_THROW_IF(!setup_.has_key(re_value_ss.str()),
                            std::logic_error,
                            "Missing key '" << re_value_ss.str()<< "' (real enumerated value #" << i << ") !");
                double re_value = setup_.fetch_real(re_value_ss.str());

                datatools::properties re_setup;
                setup_.export_and_rename_starting_with(re_setup, re_prefix_ss.str(), "");
                real_enum_value_metadata real_ev_metadata;
                real_ev_metadata.initialize(re_setup, *this);

                {
                  std::ostringstream re_default_by_ss;
                  re_default_by_ss << re_prefix_ss.str() << "default";
                  if (setup_.has_key(re_default_by_ss.str())) {
                    DT_THROW_IF(default_is_set, std::logic_error,
                                "Default enumerated real value is already set!");
                    bool def = setup_.fetch_boolean(re_default_by_ss.str());
                    if (def) {
                      this_as_default = true;
                      default_is_set = true;
                    }
                  }
                }
                add_enumerated_real_value(re_value, real_ev_metadata, this_as_default);
              }
            }

            DT_THROW_IF(_real_enumeration_.size() == 0, std::logic_error,
                        "Missing explicit supported enumeration of real values!");
          } // variable/enumeration/real

          if (is_string()) {
            /* Example:
             *
             *   string.enumerated.size : integer = 4
             *
             *   string.enumerated_0.value         : string = "Atlas"
             *   string.enumerated_0.documentation : string = "The Atlas experiment"
             *   string.enumerated_0.group         : string = "HighEnergy"
             *   string.enumerated_0.variant       : string = "if_atlas"
             *
             *   string.enumerated_1.value         : string = "CMS"
             *   string.enumerated_1.variant       : string = "Setup.CMS"
             *   string.enumerated_1.group         : string = "HighEnergy"
             *   string.enumerated_1.default       : boolean = true
             *
             *   string.enumerated_2.value         : string = "LHCb"
             *   string.enumerated_2.variant       : string = "Setup.LHCb"
             *   string.enumerated_2.group         : string = "HighEnergy"
             *
             *   string.enumerated_3.value         : string = "SuperNEMO"
             *   string.enumerated_3.variant       : string = "Setup.SuperNEMO"
             *   string.enumerated_3.group         : string = "LowEnergy"
             *
             *   string.default                    : string = "Atlas"
             */
            if (_string_enumeration_.size() == 0) {
              bool default_is_set = false;
              // Explicit list of supported string values:
              int number_of_enumerated = 0;
              if (setup_.has_key("string.enumerated.size")) {
                number_of_enumerated = setup_.fetch_positive_integer("string.enumerated.size");
              }
              for (int i = 0; i < number_of_enumerated; i++) {
                bool this_as_default = false;
                std::ostringstream se_prefix_ss;
                se_prefix_ss <<  "string.enumerated_" <<  i << ".";
                std::ostringstream se_value_ss;
                se_value_ss << se_prefix_ss.str() << "value";
                DT_THROW_IF(!setup_.has_key(se_value_ss.str()),
                            std::logic_error,
                            "Missing key '" << se_value_ss.str()<< "' (string enumerated value #" << i << ") !");
                std::string se_value = setup_.fetch_string(se_value_ss.str());

                datatools::properties se_setup;
                setup_.export_and_rename_starting_with(se_setup, se_prefix_ss.str(), "");
                string_enum_value_metadata string_ev_metadata;
                string_ev_metadata.initialize(se_setup, *this);

                {
                  std::ostringstream se_default_by_ss;
                  se_default_by_ss << se_prefix_ss.str() << "default";
                  if (setup_.has_key(se_default_by_ss.str())) {
                    DT_THROW_IF(default_is_set, std::logic_error,
                                "Default enumerated string value is already set!");
                    bool def = setup_.fetch_boolean(se_default_by_ss.str());
                    if (def) {
                      this_as_default = true;
                      default_is_set = true;
                    }
                  }
                }
                add_enumerated_string_value(se_value, string_ev_metadata, this_as_default);
              }
            }

            if (_string_enumeration_.size() == 0) {
              // Load the list of supported string values from a CSV file:
              if (setup_.has_key("string.enumerated.csv_file")) {
                const std::string & enum_csv = setup_.fetch_string("string.enumerated.csv_file");
                char csv_separator = ':';
                char csv_subseparator = ';';
                std::string regex_name_selector;
                std::string regex_group_selector;
                if (setup_.has_key("string.enumerated.csv_file.separator")) {
                  csv_separator = setup_.fetch_one_character("string.enumerated.csv_file.separator");
                }
                if (setup_.has_key("string.enumerated.csv_file.subseparator")) {
                  csv_subseparator = setup_.fetch_one_character("string.enumerated.csv_file.subseparator");
                }
                if (setup_.has_key("string.enumerated.csv_file.name_regex")) {
                  regex_name_selector = setup_.fetch_string("string.enumerated.csv_file.name_regex");
                }
                if (setup_.has_key("string.enumerated.csv_file.group_regex")) {
                  regex_group_selector = setup_.fetch_string("string.enumerated.csv_file.group_regex");
                }
                _parse_string_enumeration_from_csv_file(enum_csv,
                                                        csv_separator,
                                                        csv_subseparator,
                                                        regex_name_selector,
                                                        regex_group_selector);
                DT_THROW_IF(_string_enumeration_.size() == 0, std::logic_error,
                            "No string value was selected from the CSV file!");
              }
            }

            DT_THROW_IF(_string_enumeration_.size() == 0, std::logic_error,
                        "Missing explicit supported enumeration of string values!");

          } // variable/enumeration/string

        } // variable/enumeration

        if (is_free()) {

          if (is_string()) {

            // /* Example:
            //  *
            //  *   string.special.size      : integer = 1
            //  *   string.special_0.value   : string  = "Atlas"
            //  *   string.special_0.variant : string  = "Setup.Atlas"
            //  */
            // int number_of_special = 0;
            //  if (setup_.has_key("string.special.size")) {
            //    number_of_special = setup_.fetch_integer("string.special.size");
            //  }
            //  for (int i = 0; i < number_of_special; i++) {
            //    std::ostringstream se_value_ss;
            //    se_value_ss << "string.special_" <<  i << ".value";
            //    DT_THROW_IF(!setup_.has_key(se_value_ss.str()),
            //                std::logic_error,
            //                "Missing key '" << se_value_ss.str()
            //                << "' (string special value #" << i << ") !");
            //    std::string se_value = setup_.fetch_string(se_value_ss.str());
            //    add_enumerated_string_value(se_value);
            //    {
            //      std::ostringstream se_variant_ss;
            //      se_variant_ss << "string.special_" <<  i << ".variant";
            //      std::string se_variant;
            //      if (setup_.has_key(se_variant_ss.str())) {
            //        se_variant = setup_.fetch_string(se_variant_ss.str());
            //        associate_variant_to_string(se_value, se_variant);
            //      }
            //    }
            // }

          } // variable/free/string

        } // variable/free

        // Parse default value if not already set:
        if (!has_default_value()) {
          if (is_boolean()) {
            if (setup_.has_key("boolean.default")) {
              bool boolean_def_val = setup_.fetch_boolean("boolean.default");
              set_default_boolean(boolean_def_val);
            }
          } else if (is_integer()) {
            if (setup_.has_key("integer.default")) {
              int integer_def_val = setup_.fetch_integer("integer.default");
              set_default_integer(integer_def_val);
            }
          } else if (is_real()) {
            if (setup_.has_key("real.default")) {
              double real_def_val = setup_.fetch_real("real.default");
              if (! setup_.has_explicit_unit("real.default")) {
                real_def_val *= real_default_unit;
              }
              set_default_real(real_def_val);
            }
          } else if (is_string()) {
            if (setup_.has_key("string.default")) {
              std::string string_def_val = setup_.fetch_string("string.default");
              set_default_string(string_def_val);
            }
          }
        }
      } // variable

      if (is_fixed()) {

        // Parse fixed value:
        if (is_boolean()) {
          if (setup_.has_key("boolean.fixed")) {
            bool boolean_fixed_val = setup_.fetch_boolean("boolean.fixed");
            set_fixed_boolean(boolean_fixed_val);
          }
        } else if (is_integer()) {
          if (setup_.has_key("integer.fixed")) {
            int integer_fixed_val = setup_.fetch_integer("integer.fixed");
            set_fixed_integer(integer_fixed_val);
          }
        } else if (is_real()) {
          if (setup_.has_key("real.fixed")) {
            double real_fixed_val = setup_.fetch_real("real.fixed");
            if (! setup_.has_explicit_unit("real.fixed")) {
              real_fixed_val *= real_default_unit;
            }
            set_fixed_real(real_fixed_val);
          }
        } else if (is_string()) {
          if (setup_.has_key("string.fixed")) {
            std::string string_fixed_val = setup_.fetch_string("string.fixed");
            set_fixed_string(string_fixed_val);
          }
        }

      } // fixed

      // Checks
      if (is_variable() && is_interval()) {

        if (is_real()) {
          if (has_default_value()) {
            DT_THROW_IF (!_real_domain_.has(_real_default_, get_real_precision()),
                         std::logic_error,
                         "Default real value '" << _real_default_ << "' is not in the real domain '"
                         << _real_domain_ << "'!");
          } // default value
        } // real

        if (is_integer()) {
          if (has_default_value()) {
            DT_THROW_IF (!_integer_domain_.has(_integer_default_),
                         std::logic_error,
                         "Default integer value '" << _integer_default_
                         << "' is not in the real domain '"
                         << _integer_domain_ << "' !");
          } // default value
        } // integer

      } // variable/interval

      // Variants:

      // Import auxiliary properties:
      setup_.export_and_rename_starting_with(grab_auxiliaries(), aux_prefix(), "");

      _initialized_ = true;
      if (datatools::logger::is_trace(get_logging_priority())) {
        this->tree_dump(std::clog, "Parameter model '" + get_name() + "' : ", "[trace]: ");
      }
      return;
    }

    void parameter_model::initialize_standalone(const properties & setup_)
    {
      model_item_dict_type dummy_dict;
      initialize(setup_, dummy_dict);
      return;
    }

    void parameter_model::initialize_simple()
    {
      properties dummy_setup;
      initialize_standalone(dummy_setup);
      return;
    }

    void parameter_model::_parse_string_enumeration_from_csv_file(const std::string & csv_filename_,
                                                                  const char csv_separator_,
                                                                  const char csv_subseparator_,
                                                                  const std::string & value_regex_,
                                                                  const std::string & group_regex_)
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      // logging = datatools::logger::PRIO_DEBUG;
      std::string filename = csv_filename_;
      datatools::fetch_path_with_env(filename);
      std::ifstream fin(filename.c_str());
      DT_THROW_IF(!fin, std::runtime_error, "Cannot open string enumerated CSV file '" << csv_filename_ << "'!");
      std::string default_value;
      std::string csv_separators;
      csv_separators.push_back(csv_separator_);
      std::size_t line_counter = 0;
      while (fin && !fin.eof()) {
        bool default_value_flag = false;
        int value_rank = -1;
        std::string line;
        std::getline(fin, line);
        line_counter++;
        boost::algorithm::trim(line);
        if (line.length() == 0) {
          DT_LOG_DEBUG(logging, "Skipping blank line #" <<  line_counter);
          continue;
        } else if (line[0] == '#') {
          DT_LOG_DEBUG(logging, "Skipping comment line #" <<  line_counter);
          std::string meta_comment;
          std::istringstream liss(line);
          liss >> meta_comment;
          if (meta_comment == "#@debug") {
            logging = datatools::logger::PRIO_DEBUG;
            DT_LOG_DEBUG(logging, "Activate debug mode");
          } else if (meta_comment == "#@nodebug") {
            DT_LOG_DEBUG(logging, "Deactivate debug mode");
            logging = datatools::logger::PRIO_FATAL;
          } else {
            continue;
          }
        }
        if (line[0] == '*') {
          line = line.substr(1);
          DT_THROW_IF(!default_value.empty(), std::logic_error, "Default string enumerated value is already set!");
          DT_LOG_DEBUG(logging, "Found the default value mark at line #" << line_counter);
          default_value_flag = true;
        }
        /*
        if (line[0] == '+') {
          line = line.substr(1);
          DT_LOG_DEBUG(logging, "Found a highlight rank mark at line #" <<  line_counter);
          value_rank = string_enum_value_metadata::RANK_HIGHLIGHTED;
        } else if (line[0] == '-') {
          line = line.substr(1);
          cut_head=1;
          DT_LOG_DEBUG(logging, "Found a last rank mark at line #" <<  line_counter);
          value_rank = string_enum_value_metadata::RANK_LAST;
        } else if (line.substr(0,2) == ">>") {
          line = line.substr(2);
          cut_head=2;
          DT_LOG_DEBUG(logging, "Found a ternary rank mark at line #" <<  line_counter);
          value_rank = string_enum_value_metadata::RANK_TERNARY;
        } else if (line.substr(0,1) == ">") {
          line = line.substr(1);
          cut_head=1;
          DT_LOG_DEBUG(logging, "Found a secondary rank mark at line #" <<  line_counter);
          value_rank = string_enum_value_metadata::RANK_SECONDARY;
        }
        */
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, line, boost::is_any_of(csv_separators)); // , boost::token_compress_on);
        std::string se_value;
        if (tokens.size() > 0) {
          // CSV column 0:
          se_value = tokens[0];
          boost::algorithm::trim(se_value);
        }
        // XX remove leading '+*>>'
        DT_THROW_IF(se_value.empty(), std::logic_error, "Empty enumerated string value!");
        if (!value_regex_.empty()) {
          static const boost::regex e(value_regex_, boost::regex::extended);
          if (!boost::regex_match(se_value, e)) {
            DT_LOG_DEBUG(logging, "Skipping value '" << se_value << "' at line #" <<  line_counter);
            continue;
          }
        }
        std::string se_doc;
        if (tokens.size() > 1) {
          // CSV column 1:
          se_doc = tokens[1];
          boost::algorithm::trim(se_doc);
        }
        std::string se_group;
        if (tokens.size() > 2) {
          // CSV column 2:
          se_group = tokens[2];
          boost::algorithm::trim(se_group);
        }
        if (!group_regex_.empty()) {
          static const boost::regex e(group_regex_, boost::regex::extended);
          if (!boost::regex_match(se_group, e)) {
            DT_LOG_DEBUG(logging, "Skipping value '" << se_value << "' with unselected group '" << se_group << "' at line #" <<  line_counter);
            continue;
          }
        }
        std::string se_variants;
        if (tokens.size() > 3) {
          // CSV column 3:
          se_variants = tokens[3];
          boost::algorithm::trim(se_variants);
        }
        std::string se_metas;
        if (tokens.size() > 4) {
          // CSV column :
          se_metas = tokens[4];
          boost::algorithm::trim(se_metas);
        }
        
        // Process:
        string_enum_value_metadata strmetadata;
        DT_LOG_DEBUG(logging, "Adding a new string value '" << se_value << "'...");
        if (default_value_flag) {
          DT_LOG_DEBUG(logging,
                       "Make it the default value '" << se_value << "'...");
          default_value = se_value;
        }
        if (!se_doc.empty()) {
          DT_LOG_DEBUG(logging,
                       "Setting documentation '" << se_doc << "' to value '" << se_value << "'...");
          strmetadata.set_documentation(se_doc);
        }
        if (!se_group.empty()) {
          DT_LOG_DEBUG(logging,
                       "Setting group '" << se_group << "' to value '" << se_value << "'...");
          strmetadata.set_group(se_group);
          add_group(se_group);
        }
        if (!se_variants.empty()) {
          std::vector<std::string> variants;
          std::string csv_subseparators;
          csv_subseparators.push_back(csv_subseparator_);
          boost::split(variants, se_variants, boost::is_any_of(csv_subseparators));
          for (std::size_t iv = 0; iv < variants.size(); iv++) {
            std::string se_variant = variants[iv];
            boost::algorithm::trim(se_variant);
            if (!se_variant.empty()) {
              DT_LOG_DEBUG(logging,
                           "Associating variant '" << se_variant << "' to value '" << se_value << "'...");
              strmetadata.add_variant(se_variant);
            }
          }
        }
        if (!se_metas.empty()) {
          std::vector<std::string> metas;
          std::string csv_subseparators;
          csv_subseparators.push_back(csv_subseparator_);
          boost::split(metas, se_metas, boost::is_any_of(csv_subseparators));
          for (std::size_t iv = 0; iv < metas.size(); iv++) {
            std::string se_meta = metas[iv];
            boost::algorithm::trim(se_meta);
            if (!se_meta.empty()) {
              DT_LOG_DEBUG(logging,
                           "Associating meta '" << se_meta << "' to value '" << se_value << "'...");
              std::string rank_tok;
              if (se_meta.substr(0,5) == "rank=") {
                rank_tok = se_meta.substr(5);
              }
              if (! rank_tok.empty()) {
                boost::algorithm::trim(rank_tok);
                if (rank_tok == "highlight") {
                  value_rank=string_enum_value_metadata::RANK_HIGHLIGHTED;
                } else if (rank_tok == "first") {
                  value_rank=string_enum_value_metadata::RANK_PRIMARY;
                } else if (rank_tok == "second") {
                  value_rank=string_enum_value_metadata::RANK_SECONDARY;
                } else if (rank_tok == "third") {
                  value_rank=string_enum_value_metadata::RANK_TERNARY;
                } else if (rank_tok == "last") {
                  value_rank=string_enum_value_metadata::RANK_LAST;
                } else { 
                  value_rank=std::stoi(rank_tok);
                }
              }
            }
          }
        }
        if (default_value_flag) {
          // Automatically highlighted:
          value_rank = string_enum_value_metadata::RANK_HIGHLIGHTED;
        }
        if (value_rank >= 0) {
          strmetadata.set_rank(value_rank);
        }
        add_enumerated_string_value(se_value, strmetadata, default_value_flag);
        fin >> std::ws;
      }
      return;
    }

    void parameter_model::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
    {
      this->enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::tag << "Documentation  : ";
      if (has_documentation()) {
        out_ << "'" << _documentation_ << "'";
      } else {
        out_ << "<no>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Type           : '"
           <<  get_label_from_type(_type_) << "'" << std::endl;

      if (is_path()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Path         : '" << label_from_path_io(_path_io_) << "'"
             << std::endl;
      }

      double unit = 1.0;
      std::string display_unit_symbol;

      if (is_real() && has_real_unit_label()) {
        display_unit_symbol = units::get_default_unit_symbol_from_label(_real_unit_label_);
        unit = units::get_unit(display_unit_symbol);
      } // real/unit

      if (is_real() && has_real_unit_label()) {
        out_ << indent_ << i_tree_dumpable::tag << "Unit label : '"
             << get_real_unit_label() << "'" << std::endl;
      } // real/unit

      if (is_real() && has_real_preferred_unit()) {
        out_ << indent_ << i_tree_dumpable::tag << "Preferred unit : '"
             << get_real_preferred_unit() << "'" << std::endl;
        display_unit_symbol = get_real_preferred_unit();
        unit = units::get_unit(get_real_preferred_unit());
      } // real/unit

      if (is_real() && has_real_precision()) {
        out_ << indent_ << i_tree_dumpable::tag << "Real precision : "
             << get_real_precision() / unit;
        if (! display_unit_symbol.empty()) {
          out_ << ' ' << display_unit_symbol;
        }
        out_ << std::endl;
      } // real/precision

      out_ << indent_ << i_tree_dumpable::tag << "Mutability     : '"
           << label_from_mutability(_mutability_) << "'" << std::endl;

      if (is_fixed()) {
        out_ << indent_ << i_tree_dumpable::tag << "Has fixed value : "
             << has_fixed_value()
             << std::endl;
        out_ << indent_ << i_tree_dumpable::tag << "Fixed value : ";
        if (is_boolean()) {
          out_ << (get_fixed_boolean() ? "true" : "false");
        } else if (is_integer()) {
          out_ << get_fixed_integer();
        } else if (is_real()) {
          out_ << get_fixed_real() / unit;
          if (! display_unit_symbol.empty()) {
            out_ << ' ' << display_unit_symbol;
          }
        } else if (is_string()) {
          out_ << "'" << get_fixed_string() << "'";
        }
        out_ << std::endl;
      } // fixed

      out_ << indent_ << i_tree_dumpable::tag << "Variants       : ";
      if (_variants_.size() == 0) {
        out_ << "<none>";
      }
      out_ << std::endl;
      for (variant_dict_type::const_iterator i = _variants_.begin();
           i != _variants_.end();
           i++) {
        out_ << indent_ << i_tree_dumpable::skip_tag;
        variant_dict_type::const_iterator j = i;
        j++;
        if (j == _variants_.end()) {
          out_ << i_tree_dumpable::last_tag;
        } else {
          out_ << i_tree_dumpable::tag;
        }
        out_ << "Variant '" << i->first << "' (model '" << i->second.get_model().get_name() << "')";
        if (!i->second.has_terse_description()) {
          out_ << " : " << i->second.get_terse_description();
        }
        out_ << std::endl;
      }

      if (is_variable()) {
        out_ << indent_ << i_tree_dumpable::tag << "Variable mode  : '"
             << label_from_variable_mode(_variable_mode_) << "'" << std::endl;


        if (is_enumeration()) {

          if (is_boolean()) {
            out_ << indent_ << i_tree_dumpable::tag << "Boolean enumeration : ";
            out_ << std::endl;
            out_ << indent_ << i_tree_dumpable::skip_tag << i_tree_dumpable::tag << "Value '" << "false" << "' : ";
            out_ << std::endl;
            {
              std::ostringstream indent2;
              indent2 << indent_ << i_tree_dumpable::skip_tag << i_tree_dumpable::skip_tag;
              _false_metadata_.tree_dump(out_, "", indent2.str());
            }

            out_ << indent_ << i_tree_dumpable::skip_tag << i_tree_dumpable::last_tag << "Value '" << "true" << "'";
            out_ << std::endl;
            {
              std::ostringstream indent2;
              indent2 << indent_ << i_tree_dumpable::skip_tag << i_tree_dumpable::last_skip_tag;
              _true_metadata_.tree_dump(out_, "", indent2.str());
            }
          } // variable/enumeration/boolean

          if (is_integer()) {
            out_ << indent_ << i_tree_dumpable::tag << "Integer enumeration : ";
            if (_integer_enumeration_.size() == 0) {
              out_ << "<none>";
            }
            out_ << std::endl;
            for (integer_enum_dict_type::const_iterator iter_val = _integer_enumeration_.begin();
                 iter_val != _integer_enumeration_.end();
                 iter_val++) {
              integer_enum_dict_type::const_iterator jter_val = iter_val;
              jter_val++;
              out_ << indent_ << i_tree_dumpable::skip_tag;
              std::ostringstream indent2;
              indent2 << indent_ << i_tree_dumpable::skip_tag;
              if (jter_val == _integer_enumeration_.end()) {
                out_ << i_tree_dumpable::last_tag;
                indent2 << i_tree_dumpable::last_skip_tag;
              } else {
                out_ << i_tree_dumpable::tag;
                indent2 << i_tree_dumpable::skip_tag;
              }
              int value = iter_val->first;
              out_ << "Value '" << value << "': ";
              out_ << std::endl;
              iter_val->second.tree_dump(out_, "", indent2.str());
            }
          } // variable/enumeration/integer

          if (is_real()) {
            out_ << indent_ << i_tree_dumpable::tag << "Real enumeration : ";
            if (_real_enumeration_.size() == 0) {
              out_ << "<none>";
            }
            out_ << std::endl;
            for (real_enum_dict_type::const_iterator iter_val
                   = _real_enumeration_.begin();
                 iter_val != _real_enumeration_.end();
                 iter_val++) {
              real_enum_dict_type::const_iterator jter_val = iter_val;
              jter_val++;
              out_ << indent_ << i_tree_dumpable::skip_tag;
              std::ostringstream indent2;
              indent2 << indent_ << i_tree_dumpable::skip_tag;
              if (jter_val == _real_enumeration_.end()) {
                out_ << i_tree_dumpable::last_tag;
                indent2 << i_tree_dumpable::last_skip_tag;
              } else {
                out_ << i_tree_dumpable::tag;
                indent2 << i_tree_dumpable::skip_tag;
              }
              double value = iter_val->first;
              out_ << "Value " << value / unit;
              if (! display_unit_symbol.empty()) {
                out_ << ' ' << display_unit_symbol;
              }
              out_ << " : " << std::endl;
              iter_val->second.tree_dump(out_, "", indent2.str());
            }
          } // variable/enumeration/real

          if (is_string()) {
            out_ << indent_ << i_tree_dumpable::tag << "String enumeration : ";
            if (_string_enumeration_.size() == 0) {
              out_ << "<none>";
            }
            out_ << std::endl;
            for (string_enum_dict_type::const_iterator iter_val = _string_enumeration_.begin();
                 iter_val != _string_enumeration_.end();
                 iter_val++) {
              string_enum_dict_type::const_iterator jter_val = iter_val;
              jter_val++;
              out_ << indent_ << i_tree_dumpable::skip_tag;
              std::ostringstream indent2;
              indent2 << indent_ << i_tree_dumpable::skip_tag;
              if (jter_val == _string_enumeration_.end()) {
                out_ << i_tree_dumpable::last_tag;
                indent2 << i_tree_dumpable::last_skip_tag;
              } else {
                out_ << i_tree_dumpable::tag;
                indent2 << i_tree_dumpable::skip_tag;
              }
              const std::string & value = iter_val->first;
              out_ << "Value '" << value << "' : " << std::endl;
              iter_val->second.tree_dump(out_, "", indent2.str());
            }
          } // variable/enumeration/string

        } // variable/enumeration

        // Regions associated to variants:
        if (is_integer()) {
          if (is_interval()) {
            out_ << indent_ << i_tree_dumpable::tag << "Domain         : "
                 << get_integer_domain() << std::endl;
          } // variable/integer/variable
          if (_integer_ranges_.size()) {
            out_ << indent_ << i_tree_dumpable::tag << "Regions with associated variant: " << std::endl;
          }
          for (integer_range_enum_dict_type::const_iterator i = _integer_ranges_.begin();
               i != _integer_ranges_.end();
               i++) {
            integer_range_enum_dict_type::const_iterator j = i;
            j++;
            out_ << indent_ << i_tree_dumpable::skip_tag;
            std::ostringstream indent2;
            indent2 << indent_ << i_tree_dumpable::skip_tag;
            if (j == _integer_ranges_.end()) {
              out_ << i_tree_dumpable::last_tag;
              indent2 << i_tree_dumpable::last_skip_tag;
            } else {
              out_ << i_tree_dumpable::tag;
              indent2 << i_tree_dumpable::skip_tag;
            }
            out_ << "Integer range '" << i->first << "' : " << std::endl;
            i->second.tree_dump(out_, "", indent2.str());
          }
        } // variable/integer

        if (is_real()) {
          if (is_interval()) {
            out_ << indent_ << i_tree_dumpable::tag << "Domain         : "
                 << get_real_domain() << std::endl;
          } // variable/real/variable
          if (_real_ranges_.size()) {
            out_ << indent_ << i_tree_dumpable::tag << "Regions with associated variant: " << std::endl;
          }
          for (real_range_enum_dict_type::const_iterator i = _real_ranges_.begin();
               i != _real_ranges_.end();
               i++) {
            real_range_enum_dict_type::const_iterator j = i;
            j++;
            out_ << indent_ << i_tree_dumpable::skip_tag;
            std::ostringstream indent2;
            indent2 << indent_ << i_tree_dumpable::skip_tag;
            if (j == _real_ranges_.end()) {
              out_ << i_tree_dumpable::last_tag;
              indent2 << i_tree_dumpable::last_skip_tag;
            } else {
              out_ << i_tree_dumpable::tag;
              indent2 << i_tree_dumpable::skip_tag;
            }
            out_ << "Real range '" << i->first << "' : " << std::endl;
            i->second.tree_dump(out_, "", indent2.str());
          }

        } // variable/real

        out_ << indent_ << i_tree_dumpable::tag << "Default value  : ";
        if (has_default_value()) {
          if (is_boolean()) {
            out_ << (get_default_boolean() ? "true" : "false");
          } else if (is_integer()) {
            out_ << get_default_integer();
          } else if (is_real()) {
            out_ << get_default_real() / unit;
            if (! display_unit_symbol.empty()) {
              out_ << ' ' << display_unit_symbol;
            }
          } else if (is_string()) {
            out_ << "'" << get_default_string() << "'";
          }
        } else {
          out_ << "<none>";
        } // default/value
        out_ << std::endl;

      } // variable

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized    : " << is_initialized() << std::endl;

      return;
    }

    bool parameter_model::is_boolean_valid(const bool value_) const
    {
      if (is_fixed()) {
        return value_ == get_fixed_boolean();
      }
      return true;
    }

    bool parameter_model::is_integer_valid(const int value_) const
    {
      if (is_fixed()) {
        return value_ == get_fixed_integer();
      } else if (is_free()) {
        return true;
      } else if (is_interval()) {
        return _integer_domain_.has(value_);
      } else if (is_enumeration()) {
        return _integer_enumeration_.count(value_) != 0;
      }
      return false;
    }

    bool parameter_model::is_real_valid(const double value_) const
    {
      double epsilon = get_real_effective_precision();
      if (is_fixed()) {
        return std::abs(value_ - get_fixed_real()) <= epsilon;
      } else if (is_free()) {
        return true;
      } else if (is_interval()) {
        return _real_domain_.has(value_);
      } else if (is_enumeration()) {
        for (const auto & re_pair : _real_enumeration_) {
          if (std::abs(value_ - re_pair.first) <= epsilon) {
            return true;
          }
        }
      }
      return false;
    }

    bool parameter_model::is_string_valid(const std::string & value_) const
    {
      if (is_fixed()) {
        return value_ == get_fixed_string();
      } else if (is_free()) {
        return true;
      } else if (is_enumeration()) {
        return _string_enumeration_.count(value_) != 0;
      }
      return false;
    }

    const parameter_model::variant_dict_type & parameter_model::get_variants() const
    {
      return _variants_;
    }

    bool parameter_model::find_variants_associated_to_boolean(const bool value_, std::set<std::string> & variants_) const
    {
      variants_.clear();
      if (value_) {
        if (_true_metadata_.has_variants()) {
          variants_ = _true_metadata_.get_variants();
        }
      } else {
        if (_false_metadata_.has_variants()) {
          variants_ = _false_metadata_.get_variants();
        }
      }
      return variants_.size() > 0;
    }

    bool parameter_model::find_variants_associated_to_integer(const int value_, std::set<std::string> & variants_) const
    {
      variants_.clear();
      if (is_enumeration()) {
        integer_enum_dict_type::const_iterator found = _integer_enumeration_.find(value_);
        DT_THROW_IF (found == _integer_enumeration_.end(), std::logic_error,
                     "Integer value '" << value_ << " is not valid for parameter model '" << get_name() << "' !");
        if (found->second.has_variants()) {
          variants_ = found->second.get_variants();
        }
      }
      if (is_interval()) {
        // Check for value in the domain:
        DT_THROW_IF(!_integer_domain_.has(value_), std::logic_error,
                    "Integer value '" << value_ << " is not valid for parameter model '" << get_name() << "' !");
      }
      for (integer_range_enum_dict_type::const_iterator i = _integer_ranges_.begin();
           i != _integer_ranges_.end();
           i++) {
        DT_LOG_DEBUG(get_logging_priority(), "Integer range : " << i->first);
        if (i->first.has(value_)) {
          DT_LOG_DEBUG(get_logging_priority(), "Value [" << value_ << "] is in " << i->first);
          if (i->second.has_variants()) {
            for (const auto & variant_name : i->second.get_variants()) {
              variants_.insert(variant_name);
              DT_LOG_DEBUG(get_logging_priority(), "Associated variants : '" << variant_name << "'");
            }
          }
        }
      }
      return variants_.size() > 0;
    }

    bool parameter_model::find_variants_associated_to_real(const double value_, std::set<std::string> & variants_) const
    {
      variants_.clear();
      if (is_enumeration()) {
        double value = datatools::invalid_real();
        for (const auto & re_pair : _real_enumeration_) {
          if (std::abs(value_ - re_pair.first) <= get_real_effective_precision()) {
            value = re_pair.first;
          }
        }
        DT_THROW_IF (!datatools::is_valid(value),
                     std::domain_error,
                     "Real value '" << value_ << " is not valid for parameter model '" << get_name() << "' !");
        real_enum_dict_type::const_iterator found = _real_enumeration_.find(value);
        if (found->second.has_variants()) {
          variants_ = found->second.get_variants();
        }
      }
      if (is_interval()) {
        // Check for value in the domain:
        DT_THROW_IF(!_real_domain_.has(value_),
                    std::domain_error,
                    "Real value '" << value_ << " is not valid for parameter model '" << get_name() << "' !");
      }
      for (real_range_enum_dict_type::const_iterator i = _real_ranges_.begin();
           i != _real_ranges_.end();
           i++) {
        if (i->first.has(value_)) {
          if (i->second.has_variants()) {
            for (const auto & variant_name : i->second.get_variants()) {
              variants_.insert(variant_name);
            }
          }
        }
      }
      return variants_.size() > 0;
    }

    bool parameter_model::find_variants_associated_to_string(const std::string & value_, std::set<std::string> & variants_) const
    {
      variants_.clear();
      if (is_enumeration()) {
        string_enum_dict_type::const_iterator found = _string_enumeration_.find(value_);
        DT_THROW_IF (found == _string_enumeration_.end(), std::logic_error,
                     "String value '" << value_ << " is not valid for parameter model '" << get_name() << "' !");
        if (found->second.has_variants()) {
          variants_ = found->second.get_variants();
        }
      } else if (is_free()) {
        // Nothing for now
      }
      return variants_.size() > 0;
    }

    // static
    void parameter_model::init_ocd(datatools::object_configuration_description & ocd_)
    {

      enriched_base::init_ocd(ocd_);

      {
        configuration_property_description & cpd = ocd_.add_configuration_property_info();
        cpd.set_name_pattern("type")
          .set_from("datatools::configuration::parameter_model")
          .set_terse_description("Set the type of the parameter_model")
          .set_traits(datatools::TYPE_STRING)
          .set_mandatory(false)
          .set_long_description("Type is choosen among:                      \n"
                                "                                            \n"
                                " * \"``boolean``\" : boolean value          \n"
                                " * \"``integer``\" : signed integer value   \n"
                                " * \"``real``\" : double precision floating \n"
                                "   point real value                         \n"
                                " * \"``string``\" : character string        \n"
                                "                                            \n"
                                )
          .add_example("Set a real parameter_model::              \n"
                       "                                          \n"
                       "  type : string = \"real\"                \n"
                       "                                          \n"
                       )
          ;
      }

      {
        configuration_property_description & cpd = ocd_.add_configuration_property_info();
        cpd.set_name_pattern("mutability")
          .set_from("datatools::configuration::parameter_model")
          .set_terse_description("Set the mutability of the parameter_model")
          .set_traits(datatools::TYPE_STRING)
          .set_mandatory(false)
          .set_long_description("Type is choosen among:                    \n"
                                "                                          \n"
                                " * \"``fixed``\" : parameter is a constant\n"
                                " * \"``variable``\" : parameter value can be chnaged \n"
                                "                                          \n"
                                )
          .add_example("Set a variable parameter_model::                \n"
                       "                                          \n"
                       " mutability : string = \"variable\"       \n"
                       "                                          \n"
                       )
          ;
      }

      {
        configuration_property_description & cpd = ocd_.add_configuration_property_info();
        cpd.set_name_pattern("real.unit_label")
          .set_from("datatools::configuration::parameter_model")
          .set_terse_description("Set the unit label for a real parameter")
          .set_traits(datatools::TYPE_STRING)
          .set_mandatory(false)
          .set_long_description("The unit label is choosen from:              \n"
                                "                                             \n"
                                " * \"``time``\" : real value is a time       \n"
                                " * \"``length``\" : real value is a distance \n"
                                " * etc...                                    \n"
                                "                                             \n"
                                )

          .add_example("Set a length parameter_model::                  \n"
                       "                                          \n"
                       " real.unit_label : string = \"length\"    \n"
                       "                                          \n"
                       )
          ;
      }

      {
        configuration_property_description & cpd = ocd_.add_configuration_property_info();
        cpd.set_name_pattern("real.preferred_unit")
          .set_from("datatools::configuration::parameter_model")
          .set_terse_description("Set the preferred unit for a real parameter")
          .set_traits(datatools::TYPE_STRING)
          .set_mandatory(false)
          .set_long_description("The preferred unit is choosen from a list of supported \n"
                                "unit symbols such as:                        \n"
                                "                                             \n"
                                " * \"``length``\" : ``\"mm\"``, ``\"um\"``, ``\"cm\"``...   \n"
                                " * \"``time``\" : `\"s\"``, ``\"ms\"``       \n"
                                " * etc...                                    \n"
                                "                                             \n"
                                )
          .add_example("Set a preferred length unit::             \n"
                       "                                          \n"
                       " real.unit_label : string = \"length\"    \n"
                       " real.preferred_unit : string = \"mm\"    \n"
                       "                                          \n"
                       )
          ;
      }



      return;
    }

    /// Print in ReST format
    void parameter_model::print_rst(std::ostream & out_,
                                    const std::string & indent_,
                                    const uint32_t flags_) const
    {
      bool with_title = true;
      bool with_description  = true;
      if (flags_ & PRINT_RST_NO_TITLE) {
        with_title = false;
      }
      if (flags_ & PRINT_RST_NO_DESC) {
        with_description = false;
      }

      if (with_title) {
        std::ostringstream hdross;
        hdross << "Parameter model ``\"" << get_name()  << "\"``";
        out_ << std::setw(hdross.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << hdross.str() << std::endl;
        out_ << std::setw(hdross.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << std::endl;
        // out_ << indent_ << "=======================================================================" << std::endl;
        // out_ << indent_ << "Parameter model ``" << get_name() << "``" << std::endl;
        // out_ << indent_ << "=======================================================================" << std::endl;
        // out_ << indent_ << std::endl;
      }
      std::string indent = indent_; // + "   ";

      if (with_description) {
        if (has_terse_description()) {
          print_multi_lines(out_, get_terse_description(), indent);
        }

        if (has_documentation()) {
          out_ << indent << std::endl;
          print_multi_lines(out_, get_documentation(), indent);
          out_ << indent << std::endl;
        }

        out_ << indent << std::endl;
        out_ << indent << "* Display name: ";
        if (! has_display_name()) {
          out_ << "*none*";
        } else {
          out_ << "``\"" << get_display_name() << "\"``";
        }
        out_ << std::endl;
      } // with description

      out_ << indent << "* Type: ";
      if (! has_type()) {
        out_ << "*undefined*";
      } else {
        out_ << "``";
        out_ << ::datatools::get_label_from_type(_type_);
        out_ << "`` ";
        if (is_path()) {
          out_ << "(as ``";
          if (is_path_input()) {
            out_ << "input ";
          } else if (is_path_output()) {
            out_ << "output ";
          } else if (is_path_input_output()) {
            out_ << "input/output ";
          }
          out_ << "path``)";
        }
      }
      out_ << std::endl;

      // For real parameters:
      double unit = 1.0;
      std::string display_unit_symbol;

      if (is_real() && has_real_unit_label()) {
        display_unit_symbol = units::get_default_unit_symbol_from_label(_real_unit_label_);
        unit = units::get_unit(display_unit_symbol);
      } // real/unit

      if (is_real() && has_real_unit_label()) {
        out_ << indent << "* " << "Unit label : ``\""
             << get_real_unit_label() << "\"``" << std::endl;
      } // real/unit

      if (is_real() && has_real_preferred_unit()) {
        out_ << indent << "* " << "Preferred unit : ``\""
             << get_real_preferred_unit() << "\"``" << std::endl;
        display_unit_symbol = get_real_preferred_unit();
        unit = units::get_unit(get_real_preferred_unit());
      } // real/unit

      if (is_real() && has_real_precision()) {
        out_ << indent << "* Real precision : ``"
             << get_real_precision() / unit;
        if (! display_unit_symbol.empty()) {
          out_ << ' ' << display_unit_symbol;
        }
        out_ << "``" << std::endl;
      } // real/precision

      out_ << indent << "* Mutability : ";
      if (is_variable() || is_fixed()) {
        out_ << "``" << label_from_mutability(_mutability_) << "``";
      } else {
        out_ << "<undefined>";
      }
      out_ << std::endl;

      if (is_fixed()) {
        out_ << indent << "* Fixed value : ";
        if (is_boolean()) {
          out_ << "``" << (get_fixed_boolean() ? "true" : "false") << "``";
        } else if (is_integer()) {
          out_ << "``" << get_fixed_integer() << "``";
        } else if (is_real()) {
          out_ << "``" << get_fixed_real() / unit;
          if (! display_unit_symbol.empty()) {
            out_ << " " << display_unit_symbol;
          }
          out_ << "``";
        } else if (is_string()) {
          out_ << "``\"" << get_fixed_string() << "\"``";
        }
        out_ << std::endl;
      } // fixed

      if (_groups_.size()) {
        out_ << indent << "* Associated groups : ";
        out_ << std::endl;
        for (group_dict_type::const_iterator i = _groups_.begin();
             i != _groups_.end();
             i++) {
          group_dict_type::const_iterator j = i;
          j++;
          if (i == _groups_.begin()) {
            out_ << std::endl;
          }
          out_ << indent << "  * ``\"" << i->first
               << "\"`` ";
          const std::string & group_documentation = i->second;
          if (!group_documentation.empty()) {
            out_ << " : " << group_documentation;
          }
          out_ << std::endl;
          if (j == _groups_.end()) {
            out_ << std::endl;
          }
        }
      }

      if (_variants_.size()) {
        out_ << indent << "* Associated variants : ";
        out_ << std::endl;
        for (variant_dict_type::const_iterator i = _variants_.begin();
             i != _variants_.end();
             i++) {
          variant_dict_type::const_iterator j = i;
          j++;
          if (i == _variants_.begin()) {
            out_ << std::endl;
          }
          out_ << indent << "  * ``\"" << i->first
               << "\"`` (model: ``\"" << i->second.get_model().get_name() << "\"``)";
          if (!i->second.has_terse_description()) {
            out_ << " : " << i->second.get_terse_description();
          }
          out_ << std::endl;
          if (j == _variants_.end()) {
            out_ << std::endl;
          }
        }
      }

      if (is_variable()) {
        out_ << indent << "* Variable mode  : ``"
             << label_from_variable_mode(_variable_mode_) << "``" << std::endl;

        if (is_enumeration()) {

          if (is_boolean()) {
            out_ << indent << "* Supported boolean values:" << std::endl;
            out_ << std::endl;
            out_ << indent <<  "  * ``" << "false" << "``";
            std::ostringstream fmdss;
            _false_metadata_.print_rst(fmdss, indent + "     ");
            if (fmdss.str().length()) {
              out_ << " : " << std::endl;
              out_ << fmdss.str();
            } else {
              out_ << std::endl;
            }

            out_ << indent <<  "  * ``" << "true" << "``";
            std::ostringstream tmdss;
            _true_metadata_.print_rst(tmdss, indent + "     ");
            if (tmdss.str().length()) {
              out_ << " : " << std::endl;
              out_ << tmdss.str();
            } else {
              out_ << std::endl;
            }
            out_ << std::endl;
          } // variable/enumeration/boolean

          if (is_integer()) {
            for (integer_enum_dict_type::const_iterator iter_val
                   = _integer_enumeration_.begin();
                 iter_val != _integer_enumeration_.end();
                 iter_val++) {
              integer_enum_dict_type::const_iterator jter_val = iter_val;
              jter_val++;
              if (iter_val == _integer_enumeration_.begin()) {
                out_ << indent << "* Supported integer values:" << std::endl;
                out_ << std::endl;
              }
              int value = iter_val->first;
              out_ << indent << "  * ``" << value << "``";
              std::ostringstream vmdss;
              iter_val->second.print_rst(vmdss, indent + "     ");
              if (vmdss.str().length()) {
                out_ << " : " << std::endl;
                out_ << vmdss.str();
              } else {
                out_ << std::endl;
              }
              if (jter_val == _integer_enumeration_.end()) {
                out_ << std::endl;
              }
            }
          } // variable/enumeration/integer

          if (is_real()) {
            for (real_enum_dict_type::const_iterator iter_val
                   = _real_enumeration_.begin();
                 iter_val != _real_enumeration_.end();
                 iter_val++) {
              real_enum_dict_type::const_iterator jter_val = iter_val;
              jter_val++;
              if (iter_val == _real_enumeration_.begin()) {
                out_ << indent << "* Supported real values:" << std::endl;
                out_ << std::endl;
              }
              double value = iter_val->first;
              out_ << indent << "  * ``" << value / unit;
              if (! display_unit_symbol.empty()) {
                out_ << ' ' << display_unit_symbol;
              }
              out_ << "``";
              std::ostringstream vmdss;
              iter_val->second.print_rst(vmdss, indent + "     ");
              if (vmdss.str().length()) {
                out_ << " : " << std::endl;
                out_ << vmdss.str();
              } else {
                out_ << std::endl;
              }
              if (jter_val == _real_enumeration_.end()) {
                out_ << std::endl;
              }
            }
          } // variable/enumeration/real

          if (is_string()) {
            for (string_enum_dict_type::const_iterator iter_val
                   = _string_enumeration_.begin();
                 iter_val != _string_enumeration_.end();
                 iter_val++) {
              string_enum_dict_type::const_iterator jter_val = iter_val;
              jter_val++;
              if (iter_val == _string_enumeration_.begin()) {
                out_ << indent << "* Supported string values:" << std::endl;
                out_ << std::endl;
              }
              const std::string & value = iter_val->first;
              out_ << indent << "  * ``\"" << value << "\"`` ";
              std::ostringstream vmdss;
              iter_val->second.print_rst(vmdss, indent + "     ");
              if (vmdss.str().length()) {
                out_ << " : " << std::endl;
                out_ << vmdss.str();
              } else {
                out_ << std::endl;
              }
              if (jter_val == _string_enumeration_.end()) {
                out_ << std::endl;
              }
            }
          } // variable/enumeration/string

        } // variable/enumeration

        // Regions associated to variants:
        if (is_integer()) {

          if (is_interval()) {
            out_ << indent << "* Domain: ``" << get_integer_domain() << "``" << std::endl;
          } // variable/integer/interval

          if (_integer_ranges_.size()) {
            out_ << indent << "* Subdomains of interest: " << std::endl;
          }
          for (integer_range_enum_dict_type::const_iterator i = _integer_ranges_.begin();
               i != _integer_ranges_.end();
               i++) {
            integer_range_enum_dict_type::const_iterator j = i;
            j++;
            if (i == _integer_ranges_.begin()) {
              out_ << std::endl;
            }
            out_ << indent << "  * Range ``" << i->first << "`` : " << std::endl;
            out_ << std::endl;
            i->second.print_rst(out_, indent + "    ");
            if (j == _integer_ranges_.end()) {
              out_ << std::endl;
            }
          }
        } // variable/integer

        if (is_real()) {

          if (is_interval()) {
            out_ << indent << "* Domain: ``" << get_real_domain() << "``" << std::endl;
          } // variable/real/interval

          if (_real_ranges_.size()) {
            out_ << indent << "* Subdomains of interest: " << std::endl;
          }
          for (real_range_enum_dict_type::const_iterator i = _real_ranges_.begin();
               i != _real_ranges_.end();
               i++) {
            real_range_enum_dict_type::const_iterator j = i;
            j++;
            if (i == _real_ranges_.begin()) {
              out_ << std::endl;
            }
            out_ << indent << "  * Range ``" << i->first << "`` : " << std::endl;
            out_ << std::endl;
            i->second.print_rst(out_, indent + "    ");
            if (j == _real_ranges_.end()) {
              out_ << std::endl;
            }
          }
        } // variable/real

        if (has_default_value()) {
          out_ << indent << "* Default value  : ``";
          if (is_boolean()) {
            out_ << (get_default_boolean() ? "true" : "false");
          } else if (is_integer()) {
            out_ << get_default_integer();
          } else if (is_real()) {
            out_ << get_default_real() / unit;
            if (! display_unit_symbol.empty()) {
              out_ << ' ' << display_unit_symbol;
            }
          } else if (is_string()) {
            out_ << "\"" << get_default_string() << "\"";
          }
          out_ << "``" << std::endl;
        } // default/value

      } // variable

      // if (_variants_.size()) {
      //   out_ << indent << "* Variants description: ";
      //   out_ << std::endl;
      //   for (variant_dict_type::const_iterator i = _variants_.begin();
      //        i != _variants_.end();
      //        i++) {
      //     variant_dict_type::const_iterator j = i;
      //     j++;
      //     if (i == _variants_.begin()) {
      //       out_ << std::endl;
      //     }
      //     out_ << indent << "  * Variant ``\"" << i->first
      //          << "\"`` (model: ``\"" << i->second.get_model().get_name() << "\"``)";
      //     if (!i->second.has_terse_description()) {
      //       out_ << " : " << i->second.get_terse_description();
      //     }
      //     out_ << std::endl;
      //     if (j == _variants_.end()) {
      //       out_ << std::endl;
      //     }
      //   }
      // }

      out_ << indent << std::endl;
      return;
    }

    void parameter_model::_set_variable_mode(const variable_mode_type variable_mode_)
    {
      if (variable_mode_ == VARIABLE_MODE_DEFAULT) {
        if (is_boolean()) {
          _variable_mode_ = VARIABLE_MODE_ENUM;
        } else if (is_integer()) {
          _variable_mode_ = VARIABLE_MODE_INTERVAL;
        } else if (is_real()) {
          _variable_mode_ = VARIABLE_MODE_INTERVAL;
        } else if (is_string()) {
          _variable_mode_ = VARIABLE_MODE_ENUM;
        }
      } else {
        DT_THROW_IF(is_boolean() && variable_mode_ != VARIABLE_MODE_ENUM,
                    std::logic_error,
                    "Boolean parameter_model '" << get_name() << "' supports only fixed 'enumeration' variable_mode !");
        DT_THROW_IF(is_string() && variable_mode_ == VARIABLE_MODE_INTERVAL,
                    std::logic_error,
                    "String parameter_model '" << get_name() << "' does not support 'interval' variable_mode !");
        _variable_mode_ = variable_mode_;
      }
      return;
    }

    void parameter_model::_set_default()
    {
      _documentation_.clear();
      _type_              = TYPE_NONE;
      _path_              = false;
      _variable_mode_     = VARIABLE_MODE_UNDEFINED;
      _mutability_        = MUTABILITY_UNDEFINED;
      _has_default_value_ = false;
      _has_fixed_value_   = false;
      _variants_.clear();
      _groups_.clear();

      _boolean_default_ = false;
      _boolean_fixed_   = false;
      _false_metadata_.reset();
      _true_metadata_.reset();

      _integer_domain_.reset();
      _integer_default_   = 0;
      _integer_fixed_     = 0;
      _integer_enumeration_.clear();
      _integer_ranges_.clear();

      _real_domain_.reset();
      datatools::invalidate(_real_precision_);
      datatools::invalidate(_real_default_);
      datatools::invalidate(_real_fixed_);
      _real_unit_label_.clear();
      _real_preferred_unit_.clear();
      _real_enumeration_.clear();
      _real_ranges_.clear();

      _string_default_.clear();
      _string_fixed_.clear();
      _string_enumeration_.clear();
      _path_ = false;
      _path_io_ = PATH_UNDEFINED;
      return;
    }

    void parameter_model::_update_real_domain_()
    {
      if (!is_real()) return;
      if (!is_variable()) return;
      if (!is_interval()) return;
      if (has_real_precision()) {
        // if (! _real_domain_.has_absolute_precision()) {
        //   _real_domain_.set_absolute_precision(get_real_precision());
        // } else {
        //   // We force precision of the domain to be the requested one for the parameter_model:
        //   _real_domain_.set_absolute_precision(get_real_precision());
        //   // DT_THROW_IF(get_real_precision() != _real_domain_.get_absolute_precision(),
        //   //             std::logic_error,
        //   //             "Unmatching precision of domain '" << _real_domain_.get_absolute_precision()
        //   //             << "' for real parameter_model '" << get_name()
        //   //             << "' with requested precision '" << get_real_precision() << "' !");
        // }
      } else {
        // if (_real_domain_.has_absolute_precision()) {
        //   _real_precision_ = _real_domain_.get_absolute_precision();
        // }
      }

      if (has_real_unit_label()) {
        if (_real_domain_.has_unit_label()) {
          DT_THROW_IF(get_real_unit_label() != _real_domain_.get_unit_label(),
                      std::logic_error,
                      "Unit label of domain '" << _real_domain_.get_unit_label()
                      << "' does not match unit label of real parameter_model '" << get_name() << "' !");
        } else {
          _real_domain_.set_unit_label(get_real_unit_label());
        }
      } else {
        if (_real_domain_.has_unit_label()) {
          set_real_unit_label(_real_domain_.get_unit_label());
        }
      }

      if (has_real_preferred_unit()) {
        if (_real_domain_.has_preferred_unit()) {
        } else {
          _real_domain_.set_preferred_unit(get_real_preferred_unit());
        }
      } else {
        if (_real_domain_.has_preferred_unit()) {
          set_real_preferred_unit(_real_domain_.get_preferred_unit());
        }
      }

      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(datatools::configuration::parameter_model, ocd_)
{
  // The class name :
  ocd_.set_class_name ("datatools::configuration::parameter_model");

  // The origin of the object:
  // ocd_.set_from ("datatools::configuration::parameter_model");

  // The class terse description :
  ocd_.set_class_description ("The model of a configuration parameter");

  // The library the class belongs to :
  ocd_.set_class_library ("datatools");

  // The class detailed documentation :
  // ocd_.set_class_documentation ("");

  // Document the various configuration parameter_models :
  datatools::configuration::parameter_model::init_ocd(ocd_);

  // Set validation support
  ocd_.set_validation_support(false);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for the class :
DOCD_CLASS_SYSTEM_REGISTRATION(datatools::configuration::parameter_model, "datatools::configuration::parameter_model")
