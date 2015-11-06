// datatools/introspection/argument.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <datatools/introspection/argument.h>

// This project:
#include <datatools/introspection/method.h>

namespace datatools {

  namespace introspection {

    argument::argument()
    {
      _access_ = ACCESS_INVALID;
      _method_ = 0;
      _default_value_ = boost::any();
      return;
    }

    argument::~argument()
    {
      return;
    }

    bool argument::has_method() const
    {
      return _method_ != 0;
    }

    void argument::set_method(const method & m_)
    {
      _method_ = &m_;
      return;
    }

    void argument::reset_method()
    {
      _method_ = 0;
      return;
    }

    const method & argument::get_method() const
    {
      DT_THROW_IF(!has_method(), std::logic_error, "Method is not set!");
      return *_method_;
    }

    bool argument::has_rank() const
    {
      return _rank_ != boost::none;
    }

    std::size_t argument::get_rank() const
    {
      return _rank_.get();
    }

    void argument::set_rank(std::size_t r_)
    {
      _rank_ = r_;
      return;
    }

    bool argument::has_name() const
    {
      return _name_ != boost::none;
    }

    const std::string & argument::get_name() const
    {
      return _name_.get();
    }

    void argument::set_name(const std::string & name_)
    {
      _name_ = name_;
      return;
    }

    bool argument::has_description() const
    {
      return _description_ != boost::none;
    }

    const std::string & argument::get_description() const
    {
      return _description_.get();
    }

    void argument::set_description(const std::string & description_)
    {
      _description_ = description_;
      return;
    }

    bool argument::has_access() const
    {
      return datatools::introspection::is_valid(_access_);
    }

    access_type argument::get_access() const
    {
      return _access_;
    }

    void argument::set_access(access_type a_)
    {
      _access_ = a_;
      return;
    }

    bool argument::is_input() const
    {
      return datatools::introspection::is_input(_access_);
    }

    bool argument::is_output() const
    {
      return datatools::introspection::is_output(_access_);
    }

    bool argument::has_data_description() const
    {
      return _data_description_.is_valid();
    }

    data_description & argument::grab_data_description()
    {
      return _data_description_;
    }

    const data_description & argument::get_data_description() const
    {
      return _data_description_;
    }

    void argument::set_data_description(const data_description & dd_)
    {
      _data_description_ = dd_;
      return;
    }

    bool argument::has_default_value() const
    {
      return ! _default_value_.empty();
    }

    const boost::any & argument::get_default_value() const
    {
      DT_THROW_IF(!has_default_value(), std::logic_error,
                  "No default value!");
      return _default_value_;
    }

    void argument::set_default_value(const boost::any & any_value_)
    {
      DT_THROW_IF(any_value_.type() != get_data_description().native_type(),
                  std::logic_error,
                  "Unmatching type info!");
      _default_value_ = any_value_;
      return;
    }

    bool argument::has_default_value_str() const
    {
      return _default_value_str_ != boost::none;
    }

    const std::string & argument::get_default_value_str() const
    {
      DT_THROW_IF(!has_default_value_str(), std::logic_error,
                  "No default value string representation!");
      return _default_value_str_.get();
    }

    void argument::set_default_value_str(const std::string & dv_)
    {
      _default_value_str_ = dv_;
      return;
    }

    bool argument::is_valid() const
    {
      return has_access() && has_data_description();
    }

    void argument::initialize()
    {
      const datatools::properties dummy;
      initialize(dummy);
      return;
    }

    void argument::initialize(const datatools::properties & config_)
    {
      if (!has_access()) {
        DT_THROW_IF(! config_.has_key("access"), std::logic_error,
                    "Missing access!");
        const std::string & access_label = config_.fetch_string("access");
        access_type access;
        DT_THROW_IF(! ::datatools::introspection::from_string(access_label, access),
                    std::logic_error,
                    "Enumeration '" << camp::enumByType<access_type>().name()
                    << "' has no value labelled '" << access_label << "'!");
        set_access(access);
      }

      if (!has_data_description()) {
        datatools::properties desc_config;
        config_.export_and_rename_starting_with(desc_config, "data.", "");
        _data_description_.initialize(desc_config);
      }

      if (!has_description()) {
        if (config_.has_key("description")) {
          set_description(config_.fetch_string("description"));
        }
      }

      if (!has_name()) {
        if (config_.has_key("name")) {
          set_name(config_.fetch_string("name"));
        }
      }

      if (!has_rank()) {
        if (config_.has_key("rank")) {
          int rank = config_.fetch_integer("rank");
          DT_THROW_IF(rank < 0, std::domain_error, "Invalid rank [" << rank << "]!");
          set_rank((std::size_t) rank);
        }
      }

      if (!has_default_value()) {
        if (config_.has_key("default_value")) {
          set_default_value_str(config_.fetch_string("default_value"));
        }
      }

      return;
    }

    void argument::reset()
    {
      _access_ = ACCESS_INVALID;
      reset_method();
      _description_.reset();
      _rank_ = boost::none;
      _name_ = boost::none;
      _description_ = boost::none;
      _default_value_ = boost::any();
      _default_value_str_ = boost::none;
      return;
    }

    void argument::tree_dump(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      if (has_method()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Method : '" << get_method().get_name() << "'"
             << std::endl;
      }

      if (has_description()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Description : '" << get_description() << "'"
             << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Access : '" << ::datatools::introspection::to_string(_access_) << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Data description : "
           << std::endl;
      {
        std::ostringstream indent2;
        indent2 << indent_ << i_tree_dumpable::skip_tag;
        _data_description_.tree_dump(out_, "", indent2.str());
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Rank : ";
      if (_rank_) {
        out_ << _rank_.get();
      } else {
        out_ << "<not set>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Name : ";
      if (_name_) {
        out_ << "'" << _name_.get() << "'";
      } else {
        out_ << "<not set>";
      }
      out_ << std::endl;

      if (has_default_value()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Default value : ";
        if (has_default_value_str()) {
          out_ << "'" << get_default_value_str() << "'";
        } else {
          out_ << "<yes>";
        }
        out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Validity : " << is_valid() << std::endl;

      return;
    }

  } // namespace introspection

} // namespace datatools
