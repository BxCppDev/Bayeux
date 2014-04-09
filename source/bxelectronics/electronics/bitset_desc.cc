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
#include <electronics/bitset_desc.h>

// Third Party
// - Bayeux/datatools
#include <datatools/exception.h>
#include <datatools/logger.h>
// - Boost/Spirit
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
//#include <boost/fusion/adapted.hpp>
//#include <boost/fusion/adapted/adt/adapt_assoc_adt_named.hpp>

// This project
#include <electronics/utils.h>

// namespace electronics {
//   struct name_member;
//   struct size_member;
//   struct lsb_position_member;
//   struct default_value_member;
// }
// BOOST_FUSION_ADAPT_ASSOC_ADT_NAMED(electronics::bitset_desc,
//                                    bitset_view,
//                                    (std::string const&, std::string const&,
//                                     obj.obj.get_name(), obj.obj.set_name(val),
//                                     electronics::name_member)
//                                    (uint16_t, uint16_t,
//                                     obj.obj.get_size(), obj.obj.set_size(val),
//                                     electronics::size_member)
//                                    (uint16_t, uint16_t,
//                                     obj.obj.get_lsb_position(), obj.obj.set_lsb_position(val),
//                                     electronics::lsb_position_member)
//                                    (std::string const&, std::string const&,
//                                     obj.obj.get_default_value(), obj.obj.set_default_value(val),
//                                     electronics::default_value_member)
//                                    )


namespace electronics {

  // static
  const uint16_t & bitset_desc::invalid_lsb_position()
  {
    static const uint16_t _p = 65535;
    return _p;
  }

  bitset_desc::bitset_desc()
  {
    _initialized_ = false;
    _size_ = 0;
    _lsb_position_ = invalid_lsb_position();
    return;
  }

  bitset_desc::~bitset_desc()
  {
    return;
  }

  bool bitset_desc::is_initialized() const
  {
    return _initialized_;
  }

  void bitset_desc::initialize()
  {
    datatools::properties dummy;
    initialize(dummy);
    return;
  }

  /// Initialize
  void bitset_desc::initialize(const datatools::properties& config_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Bitset description '" << get_name() << "' is already initialized !");

    initialize_enriched_base(config_, *this);

    if (_size_ == 0) {
      if (config_.has_key("size")) {
        int sz = config_.fetch_integer("size");
        DT_THROW_IF(sz < 0,
                    std::logic_error,
                    "Bitset description '" << get_name() << "' : Invalid size '" << sz << "' !");
        set_size((uint16_t) sz);
      }
    }

    if (_lsb_position_ == invalid_lsb_position()) {
      if (config_.has_key("lsb_position")) {
        int pos = config_.fetch_integer("lsb_position");
        DT_THROW_IF(pos < 0 && pos >= invalid_lsb_position(),
                    std::logic_error,
                    "Bitset description '" << get_name() << "' : Invalid LSB position '" << pos << "' !");
        set_lsb_position((uint16_t) pos);
      }
    }

    DT_THROW_IF(_size_ == 0,
                std::logic_error,
                "Bitset description '" << get_name() << "' : Zero size !");

    DT_THROW_IF(_lsb_position_ == invalid_lsb_position(),
                std::logic_error,
                "Bitset description '" << get_name() << "' : Invalid LSB position !");

    _initialized_ = true;
    return;
  }

  void bitset_desc::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Bitset description '" << get_name() << "' is not initialized !");
    _initialized_ = false;
    _size_ = 0;
    _lsb_position_ = invalid_lsb_position();
    _default_value_.clear();
    this->::datatools::enriched_base::clear();
    return;
  }

  void bitset_desc::set_size(uint16_t sz_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Bitset description '" << get_name() << "' is already initialized !");
    if (! _default_value_.empty()) {
      DT_THROW_IF(_default_value_.length() != sz_,
                  std::range_error, "Bitset description '" << get_name() << "' : Size '" << sz_
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

  uint16_t bitset_desc::get_size() const
  {
    return _size_;
  }

  void bitset_desc::set_default_value(const std::string & dv_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Bitset description '" << get_name() << "' is already initialized !");
    static const std::string allowed_chars = "01";
    DT_THROW_IF(dv_.find_first_not_of(allowed_chars) != dv_.npos,
                std::domain_error,
                "Invalid default value for bitset description '" << get_name() << "' !");
    if (_size_ == 0) {
      set_size(dv_.length());
    } else {
      DT_THROW_IF(dv_.length() != _size_,
                  std::range_error,
                  "Invalid default value size for bitset description '" << get_name()
                  << "' of size '" << _size_ << "' !");
    }
    _default_value_ = dv_;
    return;
  }

  bool bitset_desc::has_default_value() const
  {
    return ! _default_value_.empty();
  }

  const std::string & bitset_desc::get_default_value() const
  {
    return _default_value_;
  }

  void bitset_desc::set_lsb_position(uint16_t lsb_pos_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Bitset description '" << get_name() << "' is already initialized !");
    _lsb_position_ = lsb_pos_;
    DT_LOG_DEBUG(get_logging_priority(), "LSB=" << (int) _lsb_position_);
    return;
  }

  uint16_t bitset_desc::get_lsb_position() const
  {
    return _lsb_position_;
  }

  void bitset_desc::set(uint16_t lsb_position_, uint16_t size_, const std::string & default_value_)
  {
    set_lsb_position(lsb_position_);
    set_size(size_);
    set_default_value(default_value_);
    return;
  }

  uint16_t bitset_desc::get_msb_position() const
  {
    if (! is_initialized()) return invalid_lsb_position();
    return _lsb_position_ + _size_ - 1;
  }

  void bitset_desc::make(boost::dynamic_bitset<> & dbs_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Bitset '" << get_name() << "' is not initialized !");
    boost::dynamic_bitset<> dummy(_default_value_);
    dbs_ = dummy;
    dummy.reset();
    return;
  }

  void bitset_desc::make_mask(uint16_t size_, boost::dynamic_bitset<> & dbs_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Bitset description '" << get_name() << "' is not initialized !");
    boost::dynamic_bitset<> dummy(_size_, 0x0);
    boost::dynamic_bitset<> dummy2(_size_, ((~dummy).to_ulong() << _lsb_position_));
    dbs_ = dummy2;
    dummy2.reset();
    dummy.reset();
    return;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const bitset_desc& bs_)
  {
    out_ << '[';
    if (bs_.is_initialized()) {
      out_ << bs_._size_ << ':' << bs_._lsb_position_;
      if (bs_.has_default_value()) {
        out_ << ':' << bs_._default_value_;
      }
    } else {
      out_ << '?';
    }
    out_ << ']';
    return out_;
  }

  // friend
  std::istream & operator>>(std::istream & in_, bitset_desc& bs_)
  {

    return in_;
  }

  bool bitset_desc::from_string(const std::string & s_)
  {
    /*
    using boost::spirit::qi::ushort_;
    using boost::spirit::qi::string;
    using boost::spirit::qi::lit;
    //using boost::spirit::ascii::char_;
    std::string name;
    uint16_t size = 0;
    uint16_t lsbpos = invalid_lsb_position();
    std::string defval;
    std::string::const_iterator strbegin = s_.begin();
    boost::spirit::qi::rule<std::string::const_iterator> bitset_desc_grammar;
    bitset_desc_grammar
      %= lit("[?]") |
      ('[' >> string(name) >> ':' >> ushort_(lsbpos) >> ':' >> ushort_(size) >> -(':' << string(defval))) >> ']');
    bool res = boost::spirit::qi::parse(strbegin, s_.end(), bitset_desc_grammar);
    if (res) {
      if (size > 0) {
        if (!name.empty()) set_name(name);
        set( lsbpos, size, defval);
        initialize();
      } else {
        reset();
      }
      return false;
    }
    */
    return true;
  }

  std::string bitset_desc::to_string() const
  {
    std::string dummy;
    to_string(dummy);
    return dummy;
  }

  bool bitset_desc::to_string(std::string & s_) const
  {
    if (is_initialized()) {
      //using boost::spirit::karma::char_;
      using boost::spirit::karma::ushort_;
      using boost::spirit::karma::string;
      using boost::spirit::karma::generate;
      s_.clear();
      typedef std::back_insert_iterator<std::string> output_iterator;
      output_iterator out(s_);
      boost::spirit::karma::rule<output_iterator> bitset_desc_grammar;
      bitset_desc_grammar = '[' << string(get_name())
                             << ":" << ushort_(_lsb_position_)
                             << ':' << ushort_(_size_)
                             << ':' << string(_default_value_)
                             << ']';
      return generate(out, bitset_desc_grammar);
    }
    s_ = "[?]";
    return true;
  }

  void bitset_desc::tree_dump(std::ostream& out_,
                              const std::string& title_,
                              const std::string& indent_,
                              bool inherit_) const
  {
    this->enriched_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Initialized : " << _initialized_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Size : " << (int) _size_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "LSB position : " << (int) _lsb_position_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "MSB position : " << (int) get_msb_position() << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Default value : ";
    if (_default_value_.empty()) {
      out_ << "<none>";
    } else {
      out_ << _default_value_;
    }
    out_ << std::endl;

    return;
  }

} // end of namespace electronics
