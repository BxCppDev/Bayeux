// datatools/configuration/variant_object_info.cc
/*
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/configuration/variant_object_info.h>

// Standard library:
#include <sstream>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/phoenix/phoenix.hpp>
// #include <boost/phoenix/core.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/configuration/parsers.h>

namespace datatools {

  namespace configuration {

    void variant_object_info::reset()
    {
      _type_ = OBJECT_INVALID;
      _registry_name_.clear();
      _local_path_.clear();
      _parameter_value_group_name_.clear();
      _parameter_enum_string_value_.clear();
      return;
    }

    bool variant_object_info::is_valid() const
    {
      return _type_ != OBJECT_INVALID;
    }

    bool variant_object_info::is_global() const
    {
      return is_valid() && (is_registry() || !_registry_name_.empty());
    }

    bool variant_object_info::is_local() const
    {
      return is_valid() && (!is_registry() && _registry_name_.empty());
    }

    variant_object_info::object_type
    variant_object_info::get_type() const
    {
      return _type_;
    }

    bool variant_object_info::is_registry() const
    {
      return _type_ == OBJECT_REGISTRY;
    }

    bool variant_object_info::is_variant() const
    {
      return _type_ == OBJECT_VARIANT;
    }

    bool variant_object_info::is_parameter() const
    {
      return _type_ == OBJECT_PARAMETER;
    }

    bool variant_object_info::is_parameter_value_group() const
    {
      return _type_ == OBJECT_PARAMETER_VALUE_GROUP;
    }

    bool variant_object_info::is_parameter_enum_string_value() const
    {
      return _type_ == OBJECT_PARAMETER_ENUM_STRING_VALUE;
    }

    void variant_object_info::set_registry_name(const std::string & registry_name_)
    {
      _registry_name_ = registry_name_;
      return;
    }

    void variant_object_info::make_global(const std::string & registry_name_)
    {
      set_registry_name(registry_name_);
      return;
    }

    void variant_object_info::make_registry(const std::string & registry_name_)
    {
      reset();
      _type_ = OBJECT_REGISTRY;
      _registry_name_ = registry_name_;
      return;
    }

    void variant_object_info::make_variant(const std::string & registry_name_,
                                           const std::string & variant_local_path_)

    {
      reset();
      _type_ = OBJECT_VARIANT;
      _registry_name_ = registry_name_;
      _local_path_ = variant_local_path_;
      return;
    }

    void variant_object_info::make_local_variant(const std::string & variant_local_path_)
    {
      make_variant("", variant_local_path_);
      return;
    }

    void variant_object_info::make_parameter(const std::string & registry_name_,
                                             const std::string & parameter_local_path_)

    {
      reset();
      _type_ = OBJECT_PARAMETER;
      _registry_name_ = registry_name_;
      _local_path_ = parameter_local_path_;
      return;
    }

    void variant_object_info::make_local_parameter(const std::string & parameter_local_path_)
    {
      make_parameter("", parameter_local_path_);
      return;
    }

    void variant_object_info::make_parameter_value_group(const std::string & registry_name_,
                                                         const std::string & param_local_path_,
                                                         const std::string & group_name_)
    {
      reset();
      _type_ = OBJECT_PARAMETER_VALUE_GROUP;
      _registry_name_ = registry_name_;
      _local_path_ = param_local_path_;
      _parameter_value_group_name_ = group_name_;
      return;
    }

    void variant_object_info::make_local_parameter_value_group(const std::string & param_local_path_,
                                                               const std::string & group_name_)
    {
      make_parameter_value_group("", param_local_path_, group_name_);
      return;
    }

    void variant_object_info::make_parameter_enum_string_value(const std::string & registry_name_,
                                                               const std::string & param_local_path_,
                                                               const std::string & value_)
    {
      reset();
      _type_ = OBJECT_PARAMETER_ENUM_STRING_VALUE;
      _registry_name_ = registry_name_;
      _local_path_ = param_local_path_;
      _parameter_enum_string_value_ = value_;
      return;
    }

    void variant_object_info::make_local_parameter_enum_string_value(const std::string & param_local_path_,
                                                                     const std::string & value_)
    {
      make_parameter_enum_string_value("", param_local_path_, value_);
      return;
    }

    const std::string & variant_object_info::get_registry_name() const
    {
      return _registry_name_;
    }

    const std::string & variant_object_info::get_local_path() const
    {
      return _local_path_;
    }

    const std::string & variant_object_info::get_variant_local_path() const
    {
      DT_THROW_IF(!is_variant(), std::logic_error, "Not a variant path!");
      return _local_path_;
    }

    const std::string & variant_object_info::get_parameter_local_path() const
    {
      DT_THROW_IF(!is_parameter() && !is_parameter_value_group() && !is_parameter_enum_string_value(),
                  std::logic_error, "Not a parameter path!");
      return _local_path_;
    }

    const std::string & variant_object_info::get_parameter_value_group_name() const
    {
      DT_THROW_IF(!is_parameter_value_group(), std::logic_error, "Not a parameter value group!");
      return _parameter_value_group_name_;
    }

    const std::string & variant_object_info::get_parameter_enum_string_value() const
    {
      DT_THROW_IF(!is_parameter_enum_string_value(), std::logic_error, "Not a parameter enumerated string value!");
      return _parameter_enum_string_value_;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const variant_object_info & voi_)
    {
      if (voi_.is_valid()) {
        if (voi_.is_registry()) {
          out_ << voi_._registry_name_;
        } else {
          if (voi_.is_global()) {
            out_ << voi_._registry_name_ << ':';
          }
          if (voi_.is_variant()) {
            out_ << voi_.get_variant_local_path();
          } else {
            out_ << voi_.get_parameter_local_path();
            if (voi_.is_parameter_value_group()) {
              out_ << '@' << voi_.get_parameter_value_group_name();
            } else if (voi_.is_parameter_enum_string_value()) {
              out_ << '=' << voi_.get_parameter_enum_string_value();
            }
          }
        }
      } else {
        out_ << '<invalid>';
      }
      return out_;
    }

    void variant_object_info::to_string(std::string & repr_) const
    {
      std::ostringstream out;
      out << *this;
      repr_ = out.str();
      return;
    }

    std::string variant_object_info::get_full_path() const
    {
      std::string p;
      to_string(p);
      return p;
    }

    bool variant_object_info::match_path(const std::string & path_) const
    {
      if (get_full_path() == path_) return true;
      return false;
    }


    bool variant_object_info::parse_from_string(const std::string & repr_, uint32_t flags_)
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      bool parse_global = true;
      bool parse_local = true;
      if (flags_ & PARSE_DEBUG) {
        logging = datatools::logger::PRIO_DEBUG;
      }
      if (flags_ & PARSE_NO_GLOBAL) {
        parse_global = false;
      }
      if (flags_ & PARSE_NO_LOCAL) {
        parse_local = false;
      }
      bool parsed = false;
      if (! parsed && parse_global) {
        parsed = _parse_from_string_global(repr_, logging);
      }
      if (! parsed && parse_local) {
        parsed = _parse_from_string_local(repr_, logging);
      }
      return parsed;
    }

    bool variant_object_info::_parse_from_string_local(const std::string & repr_, const datatools::logger::priority logging_)
    {
      namespace qi = boost::spirit::qi;

      {
        // Attempt to decode a local variant:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::local_variant_path_instance_grammar<std::string::const_iterator> vg;
        std::string local_variant_path;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    -qi::lit(':') >>
                                                    vg[boost::phoenix::ref(local_variant_path) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "local variant parsing fail at <" << *str_iter << ">");
        } else if (!local_variant_path.empty()) {
          make_local_variant(local_variant_path);
          DT_LOG_DEBUG(logging_, "local variant success");
          return true;
        }
      }

      {
        // Attempt to decode a local variant parameter:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::local_param_path_instance_grammar<std::string::const_iterator> pg;
        std::string registry_name;
        std::string local_param_path;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    -qi::lit(':') >>
                                                    pg[boost::phoenix::ref(local_param_path) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "local parameter parsing fail at <" << *str_iter << ">");
        } else if (!local_param_path.empty()) {
          make_local_parameter(local_param_path);
          DT_LOG_DEBUG(logging_, "local parameter success");
          return true;
        }
      }

      {
        // Attempt to decode a local parameter value group:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::local_param_path_instance_grammar<std::string::const_iterator> pg;
        datatools::configuration::group_name_instance_grammar<std::string::const_iterator> gg;
        std::string local_param_path;
        std::string value_group;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    -qi::lit(':') >>
                                                    pg[boost::phoenix::ref(local_param_path) = boost::spirit::qi::_1]
                                                    >> qi::lit('@')
                                                    >> gg[boost::phoenix::ref(value_group) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "local value group parsing fail at <" << *str_iter << ">");
        } else if (!local_param_path.empty() && !value_group.empty()) {
          make_local_parameter_value_group(local_param_path, value_group);
          DT_LOG_DEBUG(logging_, "local value group success");
          return true;
        }
      }

      {
        // Attempt to decode a local parameter enumerated string value:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::local_param_path_instance_grammar<std::string::const_iterator> pg;
        datatools::configuration::enumerated_string_value_repr_instance_grammar<std::string::const_iterator> eg;
        std::string registry_name;
        std::string local_param_path;
        std::string enum_string_value;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    -qi::lit(':') >>
                                                    pg[boost::phoenix::ref(local_param_path) = boost::spirit::qi::_1]
                                                    >> qi::lit('=')
                                                    >> eg[boost::phoenix::ref(enum_string_value) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "local enum string value parsing fail at <" << *str_iter << ">");
        } else if (!local_param_path.empty() && !enum_string_value.empty()) {
          make_local_parameter_enum_string_value(local_param_path, enum_string_value);
          DT_LOG_DEBUG(logging_, "local enum string value success");
          return true;
        }
      }

      DT_LOG_DEBUG(logging_, "Local parsing of '" << repr_ << "' failed");
      return false;
    }

    bool variant_object_info::_parse_from_string_global(const std::string & repr_, const datatools::logger::priority logging_)
    {
      namespace qi = boost::spirit::qi;
      {
        // Attempt to decode a registry:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::registry_name_instance_grammar<std::string::const_iterator> rg;
        std::string registry_name;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    rg[boost::phoenix::ref(registry_name) = boost::spirit::qi::_1]
                                                    >> -qi::lit(':')
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        //                                                   registry_name);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "registry parsing fail at <" << *str_iter << ">");
        } else if (!registry_name.empty()) {
          make_registry(registry_name);
          DT_LOG_DEBUG(logging_, "registry success");
          return true;
        }
      }

      {
        // Attempt to decode a variant:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::registry_name_instance_grammar<std::string::const_iterator> rg;
        datatools::configuration::local_variant_path_instance_grammar<std::string::const_iterator> vg;
        std::string registry_name;
        std::string local_variant_path;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    rg[boost::phoenix::ref(registry_name) = boost::spirit::qi::_1]
                                                    >> qi::lit(':')
                                                    >> vg[boost::phoenix::ref(local_variant_path) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "variant parsing fail at <" << *str_iter << ">");
        } else if (!registry_name.empty() && !local_variant_path.empty()) {
          make_variant(registry_name, local_variant_path);
          DT_LOG_DEBUG(logging_, "variant success");
          return true;
        }
      }

      {
        // Attempt to decode a variant parameter:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::registry_name_instance_grammar<std::string::const_iterator> rg;
        datatools::configuration::local_param_path_instance_grammar<std::string::const_iterator> pg;
        std::string registry_name;
        std::string local_param_path;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    rg[boost::phoenix::ref(registry_name) = boost::spirit::qi::_1]
                                                    >> qi::lit(':')
                                                    >> pg[boost::phoenix::ref(local_param_path) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "parameter parsing fail at <" << *str_iter << ">");
        } else if (!registry_name.empty() && !local_param_path.empty()) {
          make_parameter(registry_name, local_param_path);
          DT_LOG_DEBUG(logging_, "parameter success");
          return true;
        }
      }

      {
        // Attempt to decode a parameter value group:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::registry_name_instance_grammar<std::string::const_iterator> rg;
        datatools::configuration::local_param_path_instance_grammar<std::string::const_iterator> pg;
        datatools::configuration::group_name_instance_grammar<std::string::const_iterator> gg;
        std::string registry_name;
        std::string local_param_path;
        std::string value_group;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    rg[boost::phoenix::ref(registry_name) = boost::spirit::qi::_1]
                                                    >> qi::lit(':')
                                                    >> pg[boost::phoenix::ref(local_param_path) = boost::spirit::qi::_1]
                                                    >> qi::lit('@')
                                                    >> gg[boost::phoenix::ref(value_group) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "value group parsing fail at <" << *str_iter << ">");
        } else if (!registry_name.empty() && !local_param_path.empty() && !value_group.empty()) {
          make_parameter_value_group(registry_name, local_param_path, value_group);
          DT_LOG_DEBUG(logging_, "value group success");
          return true;
        }
      }

      {
        // Attempt to decode a parameter enumerated string value:
        std::string::const_iterator str_iter = repr_.begin();
        std::string::const_iterator end_iter = repr_.end();
        datatools::configuration::registry_name_instance_grammar<std::string::const_iterator> rg;
        datatools::configuration::local_param_path_instance_grammar<std::string::const_iterator> pg;
        datatools::configuration::enumerated_string_value_repr_instance_grammar<std::string::const_iterator> eg;
        std::string registry_name;
        std::string local_param_path;
        std::string enum_string_value;
        bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                   end_iter,
                                                   // Begin grammar:
                                                   (
                                                    rg[boost::phoenix::ref(registry_name) = boost::spirit::qi::_1]
                                                    >> qi::lit(':')
                                                    >> pg[boost::phoenix::ref(local_param_path) = boost::spirit::qi::_1]
                                                    >> qi::lit('=')
                                                    >> eg[boost::phoenix::ref(enum_string_value) = boost::spirit::qi::_1]
                                                    )
                                                   // End grammar
                                                   ,
                                                   boost::spirit::ascii::space);
        if (!res || str_iter != end_iter) {
          DT_LOG_DEBUG(logging_, "enum string value parsing fail at <" << *str_iter << ">");
        } else if (!registry_name.empty() && !local_param_path.empty() && !enum_string_value.empty()) {
          make_parameter_enum_string_value(registry_name, local_param_path, enum_string_value);
          DT_LOG_DEBUG(logging_, "enum string value success");
          return true;
        }
      }

      DT_LOG_DEBUG(logging_, "Global parsing of '" << repr_ << "' failed");
      return false;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
