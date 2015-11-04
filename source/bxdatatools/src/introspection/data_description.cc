// datatools/introspection/data_description.cc
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
#include <datatools/introspection/data_description.h>

// Third party:
// - Camp:
#include <camp/enum.hpp>

// This project:
#include <datatools/units.h>
#include <datatools/exception.h>

namespace datatools {

  namespace introspection {

    data_description::data_description()
    {
      _set_defaults();
      return;
    }

    data_description::~data_description()
    {
      return;
    }

    bool data_description::has_type() const
    {
      return ::datatools::introspection::is_valid(_type_);
    }

    data_type data_description::get_type() const
    {
      return _type_;
    }

    void data_description::set_type(data_type t_)
    {
      _type_ = t_;
      return;
    }

    const std::type_info & data_description::native_type() const
    {
      DT_THROW_IF(!has_type(), std::logic_error, "Invalid type!");
      if (is_scalar(get_layout())) {
        return get_type_info(_type_, "scalar");
      }
      return get_type_info(_type_, "vector");
    }

    bool data_description::has_layout() const
    {
      return ::datatools::introspection::is_valid(_layout_);
    }

    data_layout data_description::get_layout() const
    {
      return _layout_;
    }

    std::string data_description::get_layout_label() const
    {
      if (is_scalar(get_layout())) {
        return std::string("scalar");
      } else  {
        return std::string("vector");
      }
    }

    void data_description::set_layout(data_layout l_)
    {
      _layout_ = l_;
      return;
    }

    bool data_description::has_vector_fixed_size() const
    {
      return _vector_fixed_size_;
    }

    std::size_t data_description::get_vector_fixed_size() const
    {
      return _vector_fixed_size_.get();
    }

    void data_description::set_vector_fixed_size(std::size_t afsz_)
    {
      _vector_fixed_size_ = afsz_;
      return;
    }

    bool data_description::has_unit_support() const
    {
      return _unit_info_ && _unit_info_.get().is_valid();
    }

    bool data_description::has_unit_info() const
    {
      return _unit_info_;
    }

    const unit_info & data_description::get_unit_info() const
    {
      DT_THROW_IF(!has_unit_info(), std::logic_error, "No unit info is available!");
      return _unit_info_.get();
    }

    void data_description::set_unit_info(const unit_info & ui_)
    {
      DT_THROW_IF(!is_real(get_type()), std::logic_error,
                  "No unit information can be set for non-real data type!");
      _unit_info_ = ui_;
      return;
    }

    bool data_description::has_implicit_unit() const
    {
      return has_unit_info() && get_unit_info().has_implicit_unit();
    }

    const std::string & data_description::get_implicit_unit_symbol() const
    {
      DT_THROW_IF(!has_unit_info(), std::logic_error, "No unit info is available!");
      return get_unit_info().get_implicit_unit_symbol();
    }

    bool data_description::has_explicit_unit_dimension() const
    {
      return has_unit_info() && get_unit_info().has_explicit_unit_dimension();
    }

    const std::string & data_description::get_explicit_unit_dimension_label() const
    {
      DT_THROW_IF(!has_unit_info(), std::logic_error, "No unit info is available!");
      return get_unit_info().get_explicit_unit_dimension_label();
    }

    bool data_description::has_preferred_unit() const
    {
      return has_unit_info() && get_unit_info().has_preferred_unit_symbol();
    }

    const std::string & data_description::get_preferred_unit_symbol() const
    {
      DT_THROW_IF(!has_unit_info(), std::logic_error, "No unit info is available!");
      return get_unit_info().get_preferred_unit_symbol();
    }

    bool data_description::has_type_id() const
    {
      return _type_id_;
    }

    const std::string & data_description::get_type_id() const
    {
      return _type_id_.get();
    }

    void data_description::set_type_id(const std::string & uti_)
    {
      _type_id_ = uti_;
      return;
    }

    bool data_description::is_valid() const
    {
      // Type must be set:
      if (! has_type()) return false;

      // Layout must be set:
      if (! has_layout()) return false;

      if (has_unit_info()) {
        if (!get_unit_info().is_valid()) return false;
      }

      if (_layout_ == DATA_LAYOUT_VECTOR_WITH_FIXED_SIZE) {
        // Fixed size for vector must be set:
        if (! has_vector_fixed_size()) {
          return false;
        }
      }

      return true;
    }

    void data_description::initialize()
    {
      const datatools::properties dummy;
      initialize(dummy);
      return;
    }

    void data_description::initialize(const datatools::properties & config_)
    {
      if (!has_type()) {
        DT_THROW_IF(! config_.has_key("type"), std::logic_error,
                    "Missing data type!");
        const std::string & type_label = config_.fetch_string("type");
        data_type type;
        DT_THROW_IF(! ::datatools::introspection::from_string(type_label, type),
                    std::logic_error,
                    "Enumeration '" << camp::enumByType<data_type>().name()
                    << "' has no value labelled '" << type_label << "'!");
        set_type(type);
      }

      if (!has_layout()) {
        DT_THROW_IF(! config_.has_key("layout"), std::logic_error,
                    "Missing data layout!");
        const std::string & layout_label = config_.fetch_string("layout");
        data_layout layout;
        DT_THROW_IF(! ::datatools::introspection::from_string(layout_label, layout),
                    std::logic_error,
                    "Enumeration '" << camp::enumByType<data_layout>().name()
                    << "' has no value labelled '" << layout_label << "'!");
        set_layout(layout);
      }

      if (_layout_ == DATA_LAYOUT_VECTOR_WITH_FIXED_SIZE) {
        if (! has_vector_fixed_size()) {
          DT_THROW_IF(! config_.has_key("vector_fixed_size"), std::logic_error,
                      "Missing vector fixed size!");
          int vfs = config_.fetch_integer("vector_fixed_size");
          DT_THROW_IF(vfs < 1, std::logic_error, "Invalid vector fixed size (" << vfs << ")!");
          set_vector_fixed_size((std::size_t) vfs);
        }
      }

      if (::datatools::introspection::is_real(_type_)) {
        if (!has_unit_info()) {
          unit_info ui;
          datatools::properties ui_config;
          config_.export_and_rename_starting_with(ui_config, "unit.", "");
          ui.initialize(ui_config);
          set_unit_info(ui);
        }
      }

      DT_THROW_IF(! is_valid(), std::logic_error, "Invalid data description!");
      return;
    }

    void data_description::reset()
    {
      _vector_fixed_size_.reset();
      _unit_info_.reset();
      _type_id_.reset();
      _set_defaults();
      return;
    }

    void data_description::_set_defaults()
    {
      _type_ = DATA_TYPE_INVALID;
      _layout_ = DATA_LAYOUT_INVALID;
      return;
    }

    void data_description::tree_dump(std::ostream & out_,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Type : '" << ::datatools::introspection::to_string(_type_) << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Layout : '" << ::datatools::introspection::to_string(_layout_) << "'"
           << std::endl;

      if (_vector_fixed_size_) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Vector fixed size : ";
        if (_vector_fixed_size_) {
          out_ << _vector_fixed_size_.get();
        } else {
          out_ << "<not set>";
        }
        out_ << std::endl;
      }

      if (_unit_info_) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Unit information : ";
        out_ << std::endl;
        std::ostringstream indent2;
        indent2 << indent_ << i_tree_dumpable::skip_tag;
        _unit_info_.get().tree_dump(out_, "", indent2.str());
      }

      if (_type_id_) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Type identifier : ";
        if (_type_id_) {
          out_ << "'" << _type_id_.get() << "'";
        } else {
          out_ << "<not set>";
        }
        out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Validity : " << is_valid() << std::endl;

      return;
    }

    void data_description::_make_unit(const std::string & info_, const std::string & info2_)
    {
      if (! ::datatools::introspection::is_real(_type_)) return;
      if (! info_.empty()) {
        if (datatools::units::registry::const_system_registry().has_unit_from_any(info_)) {
          unit_info uinfo;
          uinfo.make_implicit_unit(info_, info2_);
          set_unit_info(uinfo);
        } else if (datatools::units::registry::const_system_registry().has_dimension(info_)) {
          unit_info uinfo;
          uinfo.make_explicit_unit_dimension(info_, info2_);
          set_unit_info(uinfo);
        }
      }
      return;
    }

    void data_description::make_scalar(data_type t_, const std::string & info_, const std::string & info2_)
    {
      reset();
      set_type(t_);
      set_layout(DATA_LAYOUT_SCALAR);
      if (::datatools::introspection::is_real(_type_)) {
        _make_unit(info_, info2_);
      }
      if (::datatools::introspection::is_user(_type_)) {
        set_type_id(info_);
      }
      initialize();
      return;
    }

    void data_description::make_fixed_size_vector(data_type t_, std::size_t size_, const std::string & info_, const std::string & info2_)
    {
      reset();
      set_type(t_);
      set_layout(DATA_LAYOUT_VECTOR_WITH_FIXED_SIZE);
      set_vector_fixed_size(size_);
      if (::datatools::introspection::is_real(_type_)) {
        _make_unit(info_, info2_);
      }
      if (::datatools::introspection::is_user(_type_)) {
        set_type_id(info_);
      }
      initialize();
      return;
    }

    void data_description::make_free_size_vector(data_type t_, const std::string & info_, const std::string & info2_)
    {
      reset();
      set_type(t_);
      set_layout(DATA_LAYOUT_VECTOR_WITH_FREE_SIZE);
      if (::datatools::introspection::is_real(_type_)) {
        _make_unit(info_, info2_);
      }
      if (::datatools::introspection::is_user(_type_)) {
        set_type_id(info_);
      }
      initialize();
      return;
    }

  } // namespace introspection

} // namespace datatools
