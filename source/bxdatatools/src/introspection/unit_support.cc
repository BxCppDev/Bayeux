// unit_support.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <datatools/introspection/unit_support.h>

// Third party:
// - Camp:
#include <camp/enum.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>

namespace datatools {

  namespace introspection {

    //! Convert an unit support type to a string
    const std::string & to_string(unit_support_type us_)
    {
      const camp::Enum & metaUnitSupportTypeEnum = camp::enumByType<unit_support_type>();
      return metaUnitSupportTypeEnum.name(us_);
    }

    //! Convert a string to an unit support type
    bool from_string(const std::string & label_, unit_support_type & us_)
    {
      us_ = UNIT_SUPPORT_INVALID;
      const camp::Enum & metaUnitSupportTypeEnum = camp::enumByType<unit_support_type>();
      if (metaUnitSupportTypeEnum.hasName(label_)) {
        us_= static_cast<unit_support_type>(metaUnitSupportTypeEnum.value(label_));
        return true;
      }
      return false;
    }

    //! Convert a string to an unit support type
    unit_support_type from_label_to_unit_support(const std::string & label_)
    {
      unit_support_type ret;
      DT_THROW_IF(!from_string(label_, ret),
                  std::logic_error,
                  "Invalid unit support type label '" << label_ << "'!");
      return ret;
    }

    //! Check the validity of an unit support type
    bool is_valid(unit_support_type us_)
    {
      return us_ != UNIT_SUPPORT_INVALID;
    }

    //! Check if an unit support type is not used
    bool is_no_unit_support(unit_support_type us_)
    {
      return us_ == UNIT_SUPPORT_NONE;
    }

    //! Check if an unit support type implies implicit unit
    bool is_implicit_unit(unit_support_type us_)
    {
      return us_ == UNIT_SUPPORT_IMPLICIT_UNIT;
    }

    //! Check if an unit support type implies explicit unit dimension
    bool is_explicit_unit_dimension(unit_support_type us_)
    {
      return us_ == UNIT_SUPPORT_EXPLICIT_DIMENSION;
    }

    unit_info::unit_info()
    {
      _us_ = UNIT_SUPPORT_INVALID;
      return;
    }

    unit_info::unit_info(unit_support_type us_, const std::string & unit_token_, const std::string & unit_token2_)
    {
      _us_ = UNIT_SUPPORT_INVALID;
      if (datatools::introspection::is_no_unit_support(us_)) {
        DT_THROW_IF(! unit_token_.empty(), std::logic_error,
                    "Unused unit information '" << unit_token_ << "'!");
        make_none();
      } else  if (datatools::introspection::is_implicit_unit(us_)) {
        make_implicit_unit(unit_token_, unit_token2_);
      } else  if (datatools::introspection::is_explicit_unit_dimension(us_)) {
        make_explicit_unit_dimension(unit_token_, unit_token2_);
      }
      return;
    }

    unit_info::~unit_info()
    {
      return;
    }

    bool unit_info::is_valid() const
    {
      if (_us_ == UNIT_SUPPORT_INVALID) return false;
      return true;
    }

    bool unit_info::has_implicit_unit() const
    {
      return _us_ == UNIT_SUPPORT_IMPLICIT_UNIT;
    }

    bool unit_info::has_explicit_unit_dimension() const
    {
      return _us_ == UNIT_SUPPORT_EXPLICIT_DIMENSION;
    }

    void unit_info::set_unit_support(unit_support_type us_)
    {
      _us_ = us_;
      return;
    }

    unit_support_type unit_info::get_unit_support() const
    {
      return _us_;
    }

    void unit_info::set_implicit_unit_symbol(const std::string & ius_)
    {
      if (!::datatools::introspection::is_valid(_us_)) {
        _us_ = UNIT_SUPPORT_IMPLICIT_UNIT;
      }
      DT_THROW_IF(!is_implicit_unit(_us_), std::logic_error, "No implicit unit support!");
      DT_THROW_IF(! datatools::units::registry::const_system_registry().has_unit_from_any(ius_),
                  std::logic_error,
                  "Unit symbol/name '" << ius_ << "' is not registered in the unit register!");
      _implicit_unit_symbol_ = ius_;
      return;
    }

    const std::string & unit_info::get_implicit_unit_symbol() const
    {
      DT_THROW_IF(!::datatools::introspection::is_implicit_unit(_us_),
                  std::logic_error,
                  "No implicit unit support!");
      return _implicit_unit_symbol_;
    }

    void unit_info::set_explicit_unit_dimension_label(const std::string & eudl_)
    {
      if (!::datatools::introspection::is_valid(_us_)) {
        _us_ = UNIT_SUPPORT_EXPLICIT_DIMENSION;
      }
      DT_THROW_IF(!::datatools::introspection::is_explicit_unit_dimension(_us_),
                  std::logic_error,
                  "No explicit unit dimension support!");
      DT_THROW_IF(! datatools::units::registry::const_system_registry().has_dimension(eudl_),
                  std::logic_error,
                  "Unit dimension label '" << eudl_ << "' is not registered in the unit register!");
      _explicit_unit_dimension_label_ = eudl_;
      return;
    }

    const std::string & unit_info::get_explicit_unit_dimension_label() const
    {
      DT_THROW_IF(!::datatools::introspection::is_explicit_unit_dimension(_us_),
                  std::logic_error,
                  "No explicit unit dimension support!");
      return _explicit_unit_dimension_label_;
    }

    bool unit_info::has_preferred_unit_symbol() const
    {
      return !_preferred_unit_symbol_.empty();
    }

    void unit_info::set_preferred_unit_symbol(const std::string & pus_)
    {
      DT_THROW_IF(!::datatools::introspection::is_implicit_unit(_us_)
                  && ! ::datatools::introspection::is_explicit_unit_dimension(_us_),
                  std::logic_error, "No unit support!");
      DT_THROW_IF(! datatools::units::registry::const_system_registry().has_unit_from_any(pus_),
                  std::logic_error,
                  "Unit symbol/name '" << pus_ << "' is not registered in the unit register!");
      const datatools::units::unit & u =
        datatools::units::registry::const_system_registry().get_unit_from_any(pus_);
      if (has_implicit_unit()) {
        const datatools::units::unit & implicit_u =
          datatools::units::registry::const_system_registry().get_unit_from_any(_implicit_unit_symbol_);
        DT_THROW_IF(u.get_dimension_label() != implicit_u.get_dimension_label(),
                    std::logic_error,
                    "Implicit preferred unit symbol/name '" << pus_
                    << "' does not match implicit unit '" << _implicit_unit_symbol_ << "'!");
      } else if (has_explicit_unit_dimension()) {
        DT_THROW_IF(u.get_dimension_label() != _explicit_unit_dimension_label_,
                    std::logic_error,
                    "Explicit preferred unit symbol/name '" << pus_
                    << "' does not match unit dimension labelled '" << _explicit_unit_dimension_label_ << "'!");
      }
      _preferred_unit_symbol_ = pus_;
      return;
    }

    const std::string & unit_info::get_preferred_unit_symbol() const
    {
      DT_THROW_IF(!has_preferred_unit_symbol(), std::logic_error, "No preferred unit symbol is set!");
      return _preferred_unit_symbol_;
    }

    void unit_info::make_none()
    {
      reset();
      _us_ = UNIT_SUPPORT_NONE;
      _implicit_unit_symbol_.clear();
      _explicit_unit_dimension_label_.clear();
      _preferred_unit_symbol_.clear();
      return;
    }

    void unit_info::make_implicit_unit(const std::string & unit_symbol_,
                                       const std::string & preferred_unit_symbol_)
    {
      reset();
      _us_ = UNIT_SUPPORT_IMPLICIT_UNIT;
      set_implicit_unit_symbol(unit_symbol_);
      if (!preferred_unit_symbol_.empty()) {
        set_preferred_unit_symbol(preferred_unit_symbol_);
      }
      return;
    }

    void unit_info::make_explicit_unit_dimension(const std::string & unit_dimension_label_,
                                                 const std::string & preferred_unit_symbol_)
    {
      reset();
      _us_ = UNIT_SUPPORT_EXPLICIT_DIMENSION;
      set_explicit_unit_dimension_label(unit_dimension_label_);
      if (!preferred_unit_symbol_.empty()) {
        set_preferred_unit_symbol(preferred_unit_symbol_);
      }
      return;
    }

    void unit_info::initialize(const datatools::properties & config_)
    {
      reset();

      if (_us_ == UNIT_SUPPORT_INVALID) {
        if (config_.has_key("support")) {
          const std::string & us_label = config_.fetch_string("support");
          unit_support_type us = from_label_to_unit_support(us_label);
          DT_THROW_IF(!::datatools::introspection::is_valid(us), std::logic_error, "Invalid unit support type '" << us_label << "'!");
          set_unit_support(us);
        }
      }

      if (has_implicit_unit() && _implicit_unit_symbol_.empty()) {
        if (config_.has_key("implicit_unit")) {
          const std::string & ius = config_.fetch_string("implicit_unit");
          set_implicit_unit_symbol(ius);
        }
      }

      if (has_explicit_unit_dimension() && _explicit_unit_dimension_label_.empty()) {
        if (config_.has_key("explicit_unit_dimension")) {
          const std::string & eudl = config_.fetch_string("explicit_unit_dimension");
          set_explicit_unit_dimension_label(eudl);
        }
      }

      if (has_implicit_unit() || has_explicit_unit_dimension()) {
        if (!has_preferred_unit_symbol()) {
          if (config_.has_key("preferred_unit")) {
            const std::string & pus = config_.fetch_string("preferred_unit");
            set_preferred_unit_symbol(pus);
          }
        }
      }

      if (_us_ == UNIT_SUPPORT_INVALID) {
        // Default: no unit support
        _us_ = UNIT_SUPPORT_NONE;
      }
      // DT_THROW_IF(!is_valid(), std::logic_error, "Invalid unit info!");
      return;
    }

    void unit_info::export_to_config(datatools::properties & config_,
                                     uint32_t flags_,
                                     const std::string & prefix_) const
    {
      if (flags_ & UI_XC_UNIT_SUPPORT) {
        config_.store_string(prefix_ + "support", to_string(_us_), "Type of unit support for the target parameter");

        if (has_implicit_unit()) {
          config_.store_string(prefix_ + "implicit_unit", get_implicit_unit_symbol(), "Implicit unit for the target parameter");
        } else if (has_explicit_unit_dimension()) {
          config_.store_string(prefix_ + "explicit_unit_dimension", get_explicit_unit_dimension_label(), "Explicit unit dimension for the target parameter");
        }
      }

      if (has_preferred_unit_symbol()) {
        if (flags_ & UI_XC_PREFERRED_UNIT) {
          config_.store_string(prefix_ + "preferred_unit", get_preferred_unit_symbol(), "Preferred unit's symbol for the target parameter");
        }
      }

      return;
    }

    void unit_info::reset()
    {
      _us_ = UNIT_SUPPORT_INVALID;
      _implicit_unit_symbol_.clear();
      _explicit_unit_dimension_label_.clear();
      _preferred_unit_symbol_.clear();
      return;
    }

    void unit_info::tree_dump(std::ostream & out_,
                              const std::string & title_,
                              const std::string & indent_,
                              bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Unit support : '" << ::datatools::introspection::to_string(_us_) << "'"
           << std::endl;

      if (has_implicit_unit()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Implicit unit : '" << _implicit_unit_symbol_ << "'"
             << std::endl;
      } if (has_explicit_unit_dimension()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Explicit unit dimension : '" << _explicit_unit_dimension_label_ << "'"
             << std::endl;
      }
      if (has_preferred_unit_symbol()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Preferred unit : '" << _preferred_unit_symbol_ << "'"
             << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Validity : " << is_valid() << std::endl;

      return;
    }

    bool unit_info::operator==(const unit_info & ui_) const
    {
      if (_us_ != ui_._us_) return false;
      if (_implicit_unit_symbol_ != ui_._implicit_unit_symbol_) return false;
      if (_explicit_unit_dimension_label_ != ui_._explicit_unit_dimension_label_) return false;
      if (_preferred_unit_symbol_ != ui_._preferred_unit_symbol_) return false;
      return false;
    }

  } // namespace introspection

} // namespace datatools
