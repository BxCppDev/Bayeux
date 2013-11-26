// register_model_base.cc
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
#include "electronics/register_model_base.h"

// Third Party
// - Bayeux/datatools
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace electronics {

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(register_model_base, "electronics::register_model_base");

  register_model_base::register_model_base()
  {
    _size_ = SIZE_UNDEFINED;
    _readable_ = true;
    _writable_ = true;
    // _address_ = 0;
    return;
  }

  register_model_base::~register_model_base()
  {
    if (is_initialized()) {
      reset();
    }
    _mask_.reset(0);
    return;
  }

  /// Set read access flag
  void register_model_base::set_readable(bool r_)
  {
    _readable_ = r_;
    return;
  }

  /// Check read access flag
  bool register_model_base::is_readable() const
  {
    return _readable_;
  }

  /// Set write access flag
  void register_model_base::set_writable(bool w_)
  {
    _writable_ = w_;
    return;
  }

  /// Check write access flag
  bool register_model_base::is_writable() const
  {
    return _writable_;
  }

  /// Set the size
  void register_model_base::set_size(uint8_t sz_)
  {
    switch(sz_) {
    case SIZE_8:
    case SIZE_16:
    case SIZE_24:
    case SIZE_32:
    case SIZE_48:
    case SIZE_64:
      _size_ = sz_;
      break;
    default:
      DT_THROW_IF(true, std::domain_error,
                  "Invalid register size '" << sz_ << "' !");
    }
    _mask_.resize(sz_, false);
    return;
  }

  /// Get the size
  uint8_t register_model_base::get_size() const
  {
    return _size_;
  }

  /// Get the effective size
  uint8_t register_model_base::get_effective_size() const
  {
    uint32_t eff_size = 0;
    for (bitsets_dict_type::const_iterator i = _bitsets_.begin();
         i != _bitsets_.end();
         i++) {
      eff_size += i->second.get_size();
    }
    return eff_size;
  }

  /// Get the register mask
  const boost::dynamic_bitset<> & register_model_base::get_mask() const
  {
    return _mask_;
  }

  /// Add a bitset
  void register_model_base::add_bitset(const std::string & name_, const bitset_desc & bs_)
  {
    datatools::logger::priority prio = datatools::logger::PRIO_DEBUG;
    DT_THROW_IF(name_.empty(),
                std::logic_error,
                "Register '" << get_name() << "' cannot host an anonymous bitset !");
    DT_THROW_IF(_bitsets_.find(name_) != _bitsets_.end(),
                std::logic_error,
                "Register '" << get_name() << "' already has a bitset named '" << name_ << "' !");
    DT_THROW_IF(bs_.get_msb_position() >= _size_,
                std::range_error,
                "Register '" << get_name()
                << "'  is too small to address bitset named '" << name_ << "' ! ");
    DT_LOG_DEBUG(prio, "Name='" << name_ << "'"
                 << " LSB=" << (int) bs_.get_lsb_position()
                 << " MSB=" << (int) bs_.get_msb_position());
    for (int ibit = bs_.get_lsb_position();
         ibit <= bs_.get_msb_position();
         ibit++) {
      DT_THROW_IF(_mask_[ibit],
                  std::domain_error,
                  "Register '" << get_name()
                  << "' : Collision at bit '" << ibit << "' for bitset named '" << name_ << "' ! ");
      _mask_[ibit] = true;
      DT_LOG_DEBUG(prio, "  mask=" << _mask_);
    }
    _bitsets_[name_] = bs_;
    return;
  }

  /// Add a bitset
  void register_model_base::add_bitset(const std::string & name_,
                                 uint8_t lsb_position_,
                                 uint8_t size_,
                                 const std::string & default_value_)
  {
    bitset_desc bs;
    bs.set_lsb_position(lsb_position_);
    bs.set_size(size_);
    bs.set_default_value(default_value_);
    this->add_bitset(name_, bs);
    bs.reset();
    return;
  }

  void register_model_base::append_bitset(const std::string & name_,
                                    uint8_t size_,
                                    const std::string & default_value_)
  {
    boost::dynamic_bitset<> dummy(_mask_);
    dummy.flip();
    uint8_t lsb_pos = dummy.find_first();
    DT_THROW_IF(lsb_pos == dummy.npos, std::range_error,
                "No more bits available in the register for bitset '" << name_ << "' !");
    dummy.reset();
    add_bitset(name_, lsb_pos, size_, default_value_);
    return;
  }

  /// Build a dynamic bitset from the register description
  void register_model_base::make(boost::dynamic_bitset<> & dbs_) const
  {
    DT_THROW_IF(!is_initialized(),
                std::logic_error,
                "Register description is not initialized !");
    dbs_.resize(_size_, false);
    for (bitsets_dict_type::const_iterator i = _bitsets_.begin();
         i != _bitsets_.end();
         i++) {
      for (int ibit = 0;
           ibit <= i->second.get_size();
           ibit++) {
        const std::string & defval = i->second.get_default_value();
          if ( defval[defval.length() - ibit -1] == '1') {
          dbs_[i->second.get_lsb_position() + ibit] = true;
        }
      }
    }
    return;
  }

  void register_model_base::initialize(const datatools::properties& config_,
                                 component_pool_type& components_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Register '" << get_name() << "' is already initialized !");

    this->_register_initialize(config_, components_);

    _set_initialized(true);
    return;
  }

  void register_model_base::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Register '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    this->_register_reset();
    return;
  }

  bool register_model_base::allow_embedded_components() const
  {
    return false;
  }

  void register_model_base::_register_reset()
  {
    _bitsets_.clear();
    _mask_.reset();
    _readable_ = true;
    _writable_ = true;
    _size_ = SIZE_UNDEFINED;
    this->_component_reset();
    return;
  }

  void register_model_base::_register_initialize(const datatools::properties& config_,
                                           component_pool_type& components_)
  {
    this->_component_initialize(config_, components_);

    if (_size_ == SIZE_UNDEFINED) {
      if (config_.has_key("register.size")) {
        int sz = config_.fetch_integer("register.size");
        DT_THROW_IF(sz < 0,
                    std::logic_error,
                    "Register '" << get_name() << "' : Invalid size '" << sz << "' !");
        set_size((uint8_t) sz);
      }
    }
    DT_THROW_IF(_size_ == SIZE_UNDEFINED,
                std::logic_error,
                "Register '" << get_name() << "' has no valid size !");

    if (config_.has_key("register.readable")) {
      set_readable(config_.fetch_boolean("register.readable"));
    }

    if (config_.has_key("register.writable")) {
      set_readable(config_.fetch_boolean("register.writable"));
    }

    if (! _bitsets_.size()) {
      std::vector<std::string> bitsets_labels;
      if (config_.has_key("register.bitsets")) {
        config_.fetch("register.bitsets", bitsets_labels);
      }
      for (int i = 0; i < bitsets_labels.size(); i++) {
        std::ostringstream oss1;
        oss1 << "register.bitsets." << bitsets_labels[i] << ".";
        bitset_desc bsd;
        bsd.set_name(bitsets_labels[i]);
        datatools::properties bscfg;
        config_.export_and_rename_starting_with(bscfg, oss1.str(), "");
        bsd.initialize(bscfg);
        add_bitset(bitsets_labels[i], bsd);
      }
    }

    DT_THROW_IF(! _bitsets_.size(),
                std::logic_error,
                "Register '" << get_name() << "' has no bitset layout !");
    return;
  }

  void register_model_base::tree_dump(std::ostream& out_,
                                const std::string& title_,
                                const std::string& indent_,
                                bool inherits_) const
  {
    this->component_model_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Size : " << (int) _size_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Mask : '" << _mask_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Readable : " << _readable_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Writable : " << _writable_ << std::endl;

    return;
  }

} // namespace electronics
