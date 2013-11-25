// biset_desc.cc
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/electronics.
//
// Bayeux/electronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/electronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/electronics.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include "electronics/bitset_desc.h"

// Third Party
// - Bayeux/datatools
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace electronics {

  bitset_desc::bitset_desc()
  {
    _size_ = 0;
    _lsb_position_ = INVALID_BIT_POSITION;
    return;
  }

  bitset_desc::~bitset_desc()
  {
    return;
  }

  /// Initialize
  void bitset_desc::initialize(const datatools::properties& config_)
  {
    DT_THROW_IF(is_valid(),
                std::logic_error,
                "Bitset description '" << get_name() << "' is already initialized !");

    if (_size_ == 0) {
      if (config_.has_key("size")) {
        int sz = config_.fetch_integer("size");
        DT_THROW_IF(sz < 0,
                    std::logic_error,
                    "Register '" << get_name() << "' : Invalid size '" << sz << "' !");
        set_size((uint8_t) sz);
      }
    }

    if (_lsb_position_ == INVALID_BIT_POSITION) {
      if (config_.has_key("lsb_position")) {
        int pos = config_.fetch_integer("lsb_position");
        DT_THROW_IF(pos < 0 && pos >= INVALID_BIT_POSITION,
                    std::logic_error,
                    "Register '" << get_name() << "' : Invalid LSB position '" << pos << "' !");
        set_lsb_position((uint8_t) pos);
      }
    }

    if (!has_name()) {
      if (config_.has_key("name")) {
        set_name(config_.fetch_string("name"));
      }
    }

    if (!has_display_name()) {
      if (config_.has_key("display_name")) {
        set_display_name(config_.fetch_string("display_name"));
      }
    }

    if (!has_terse_description()) {
      if (config_.has_key("description")) {
        set_terse_description(config_.fetch_string("description"));
      }
    }

    config_.export_and_rename_starting_with(grab_auxiliaries(), "auxiliaries.", "");

    return;
  }

  void bitset_desc::reset()
  {
    _size_ = 0;
    _lsb_position_ = 64;
    _default_value_.clear();
    this->advanced_object::clear_advanced();
    return;
  }

  bool bitset_desc::is_valid() const
  {
    return _size_ > 0 && _lsb_position_ < 64;
  }

  void bitset_desc::set_size(uint8_t sz_)
  {
    if (! _default_value_.empty()) {
      DT_THROW_IF(_default_value_.length() != sz_,
                  std::range_error, "Bitset size '" << sz_
                  << "' is not compatible with default value '" << _default_value_ << "' !");
    }
    _size_ = sz_;
    if (_default_value_.empty()) {
      for (int i = 0; i < _size_; i++) {
        _default_value_ += "0";
      }
    }
    return;
  }

  uint8_t bitset_desc::get_size() const
  {
    return _size_;
  }

  void bitset_desc::set_default_value(const std::string & dv_)
  {
    static const std::string allowed_chars = "01";
    DT_THROW_IF(dv_.find_first_not_of(allowed_chars) != dv_.npos,
                std::domain_error,
                "Invalid default value for bitset '" << get_name() << "' !");
    if (_size_ == 0) {
      _size_ = dv_.length();
    } else {
      DT_THROW_IF(dv_.length() != _size_,
                  std::range_error,
                  "Invalid default value size for bitset '" << get_name()
                  << "' of size '" << _size_ << "' !");
    }
    _default_value_ = dv_;
    return;
  }

  const std::string & bitset_desc::get_default_value() const
  {
    return _default_value_;
  }

  void bitset_desc::set_lsb_position(uint8_t lsb_pos_)
  {
    _lsb_position_ = lsb_pos_;
    DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "LSB=" << (int) _lsb_position_);
    return;
  }

  uint8_t bitset_desc::get_lsb_position() const
  {
    return _lsb_position_;
  }

  void bitset_desc::set(uint8_t lsb_position_, uint8_t size_, const std::string & default_value_)
  {
    reset();
    set_lsb_position(lsb_position_);
    set_size(size_);
    set_default_value(default_value_);
    return;
  }

  uint8_t bitset_desc::get_msb_position() const
  {
    if (! is_valid()) return INVALID_BIT_POSITION;
    return _lsb_position_ + _size_ - 1;
  }

  void bitset_desc::make(boost::dynamic_bitset<> & dbs_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Bitset '" << get_name()
                  << "' is not valid !");
    boost::dynamic_bitset<> dummy(_default_value_);
    dbs_ = dummy;
    dummy.reset();
    return;
  }

  void bitset_desc::make_mask(uint8_t size_, boost::dynamic_bitset<> & dbs_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Bitset '" << get_name()
                  << "' is not valid !");
    DT_THROW_IF(get_msb_position() >= size_, std::logic_error, "Bitset '" << get_name()
                  << "' is not valid !");
    boost::dynamic_bitset<> dummy(_size_, 0x0);
    boost::dynamic_bitset<> dummy2(_size_, ((~dummy).to_ulong() << _lsb_position_));
    dbs_ = dummy2;
    dummy2.reset();
    dummy.reset();
    return;
  }

  void bitset_desc::tree_dump(std::ostream& out_,
                              const std::string& title_,
                              const std::string& indent_,
                              bool inherit_) const
  {
    this->advanced_object::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Valid : " << is_valid() << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Size : " << (int) _size_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "LSB position : " << (int) _lsb_position_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "MSB position : " << (int) get_msb_position() << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Default value : " << _default_value_ << std::endl;

    return;
  }

} // end of namespace electronics
