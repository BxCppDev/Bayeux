// -*- mode: c++ ; -*-
/* geom_id.cc
 */

#include <geomtools/geom_id.h>

#include <stdexcept>
#include <sstream>

#include <datatools/exception.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (geom_id,"geomtools::geom_id")

  const uint32_t geom_id::INVALID_TYPE; //           = 0xFFFFFFFF;
  const uint32_t geom_id::INVALID_ADDRESS; //        = 0xFFFFFFFF;
  const uint32_t geom_id::ANY_ADDRESS; //            = 0xFFFFFFFE;
  const uint32_t geom_id::DEFAULT_ADDRESS_DEPTH; //  = 10;
  const uint32_t geom_id::UNIVERSE_TYPE; //          = 0;
  const uint32_t geom_id::WORLD_TYPE ; //            = geom_id::UNIVERSE_TYPE;

  const char geom_id::IO_ID_OPEN           = '[';
  const char geom_id::IO_TYPE_INVALID      = '?';
  const char geom_id::IO_ID_SEPARATOR      = ':';
  const char geom_id::IO_ADDRESS_INVALID   = '?';
  const char geom_id::IO_ADDRESS_SEPARATOR = '.';
  const char geom_id::IO_ADDRESS_ANY       = '*';
  const char geom_id::IO_ID_CLOSE          = ']';

  bool geom_id::is_type (uint32_t type_) const
  {
    return _type_ == type_;
  }

  void geom_id::set_depth (uint32_t depth_)
  {
    uint32_t sz = _addresses_.size ();
    if (depth_ > sz) {
      for (size_t i = sz; i < depth_; i++) {
        _addresses_.push_back (INVALID_ADDRESS);
      }
    } else {
      _addresses_.resize (depth_);
      for (size_t i = sz; i < depth_; i++) {
        _addresses_.push_back (INVALID_ADDRESS);
      }
    }
    return;
  }

  uint32_t geom_id::get_depth () const
  {
    return _addresses_.size ();
  }

  uint32_t geom_id::get (int i_) const
  {
    // DT_THROW_IF(_type_ == INVALID_TYPE,
    //             std::logic_error,
    //             "Invalid type geometry ID!");
    // DT_THROW_IF(i_ < 0 || i_ >= _addresses_.size(),
    //             std::range_error,
    //             "Invalid address index [" << i_ << "] from geom ID = " << *this << "!");
    // return _addresses_[i_];
    return _addresses_.at(i);
  }

  void geom_id::reset_address ()
  {
    for (size_t i = 0; i < _addresses_.size (); i++) {
      _addresses_.at(i) = geom_id::INVALID_ADDRESS;
    }
    return;
  }

  void geom_id::set (int i_, uint32_t val_)
  {
    DT_THROW_IF (i_ < 0, std::logic_error, "Invalid sub-address index !");
    if (i_ < (int)_addresses_.size ()) {
      _addresses_[i_] = val_;
    } else {
      for (int j = _addresses_.size (); j <= i_; j++) {
        uint32_t v = geom_id::INVALID_ADDRESS;
        _addresses_.push_back (v);
      }
      _addresses_[i_] = val_;
    }
    return;
  }

  void geom_id::set_invalid (int i_)
  {
    set (i_, INVALID_ADDRESS);
    return;
  }

  void geom_id::set_any (int i_)
  {
    set (i_, ANY_ADDRESS);
    return;
  }

  bool geom_id::is_invalid (int i_) const
  {
    return get (i_) == INVALID_ADDRESS;
  }

  bool geom_id::is_any (int i_) const
  {
    return get (i_) == ANY_ADDRESS;
  }

  uint32_t geom_id::get_type () const
  {
    return _type_;
  }

  void geom_id::set_type (uint32_t new_value_)
  {
    _type_ = new_value_;
    return;
  }

  void geom_id::set_address (uint32_t si0_,
                             uint32_t si1_,
                             uint32_t si2_,
                             uint32_t si3_,
                             uint32_t si4_,
                             uint32_t si5_,
                             uint32_t si6_,
                             uint32_t si7_,
                             uint32_t si8_,
                             uint32_t si9_)
  {
    //if (si0_ == INVALID_ADDRESS) return;
    set (0, si0_);
    if (si1_ == INVALID_ADDRESS) return;
    set (1, si1_);
    if (si2_ == INVALID_ADDRESS) return;
    set (2, si2_);
    if (si3_ == INVALID_ADDRESS) return;
    set (3, si3_);
    if (si4_ == INVALID_ADDRESS) return;
    set (4, si4_);
    if (si5_ == INVALID_ADDRESS) return;
    set (5, si5_);
    if (si6_ == INVALID_ADDRESS) return;
    set (6, si6_);
    if (si7_ == INVALID_ADDRESS) return;
    set (7, si7_);
    if (si8_ == INVALID_ADDRESS) return;
    set (8, si8_);
    if (si9_ == INVALID_ADDRESS) return;
    set (9, si9_);
    return;
  }

  // Constructor :
  geom_id::geom_id ()
  {
    _type_ = geom_id::INVALID_TYPE;
    _addresses_.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    return;
  }

    /// Constructor
  geom_id::geom_id (uint32_t type_,
                    uint32_t si0_)
  {
    _type_ = geom_id::INVALID_TYPE;
    set_type (type_);
    _addresses_.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    set_address (si0_);
    return;
  }

  /// Constructor
  geom_id::geom_id (uint32_t type_,
                    uint32_t si0_,
                    uint32_t si1_)
  {
    _type_ = geom_id::INVALID_TYPE;
    set_type (type_);
    _addresses_.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    set_address (si0_,
                 si1_);
    return;
  }

  /// Constructor
  geom_id::geom_id (uint32_t type_,
                    uint32_t si0_,
                    uint32_t si1_,
                    uint32_t si2_)
  {
    _type_ = geom_id::INVALID_TYPE;
    set_type (type_);
    _addresses_.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    set_address (si0_,
                 si1_,
                 si2_);
    return;
  }

  /// Constructor
  geom_id::geom_id (uint32_t type_,
                    uint32_t si0_,
                    uint32_t si1_,
                    uint32_t si2_,
                    uint32_t si3_)
  {
    _type_ = geom_id::INVALID_TYPE;
    set_type (type_);
    _addresses_.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    set_address (si0_,
                 si1_,
                 si2_,
                 si3_);
    return;
  }

  // Constructor :
  geom_id::geom_id (uint32_t type_,
                    uint32_t si0_,
                    uint32_t si1_,
                    uint32_t si2_,
                    uint32_t si3_,
                    uint32_t si4_,
                    uint32_t si5_,
                    uint32_t si6_,
                    uint32_t si7_,
                    uint32_t si8_,
                    uint32_t si9_)
  {
    _type_ = geom_id::INVALID_TYPE;
    set_type (type_);
    _addresses_.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    set_address (si0_,
                 si1_,
                 si2_,
                 si3_,
                 si4_,
                 si5_,
                 si6_,
                 si7_,
                 si8_,
                 si9_);
    return;
  }

  // dtor:
  geom_id::~geom_id ()
  {
    _addresses_.clear ();
    return;
  }

  void geom_id::reset ()
  {
    _type_ = INVALID_TYPE;
    _addresses_.clear ();
    _addresses_.reserve (DEFAULT_ADDRESS_DEPTH);
    return;
  }

  void geom_id::invalidate ()
  {
    _type_ = INVALID_TYPE;
    for (size_t i = 0; i < _addresses_.size(); i++) {
      _addresses_[i] = INVALID_ADDRESS;
    }
    return;
  }

  bool geom_id::is_valid () const
  {
    if (_type_ == INVALID_TYPE) {
      return false;
    }
    if (_addresses_.size () == 0) {
      return false;
    }
    for (size_t i = 0; i < _addresses_.size (); i++) {
      if (_addresses_[i] == INVALID_ADDRESS) {
        return false;
      }
    }
    return true;
  }

  bool geom_id::is_complete () const
  {
    if (_type_ == INVALID_TYPE) {
      return false;
    }
    if (_addresses_.size () == 0) {
      return false;
    }
    for (size_t i = 0; i < _addresses_.size (); i++) {
      if (_addresses_[i] == INVALID_ADDRESS) {
        return false;
      }
      if (_addresses_[i] == ANY_ADDRESS) {
        return false;
      }
    }
    return true;
  }

  void geom_id::inherits_from (const geom_id & source_)
  {
    DT_THROW_IF (this->_addresses_.size () < source_._addresses_.size (),
                 std::logic_error, "Incompatible address depth !");
    for (size_t i = 0; i < source_._addresses_.size (); i++) {
      set (i, source_._addresses_[i]);
    }
    return;
  }

  void geom_id::extract_to (geom_id & target_) const
  {
    DT_THROW_IF (this->_addresses_.size () < target_._addresses_.size (),
                 std::logic_error, "Incompatible address depth !");
    for (size_t i = 0; i < target_._addresses_.size (); i++) {
      target_.set (i, this->_addresses_[i]);
    }
    return;
  }

  void geom_id::extract_from (const geom_id & source_)
  {
    source_.extract_to (*this);
    return;
  }

  std::ostream & operator<< (std::ostream & out_, const geom_id & id_)
  {
    out_ << geom_id::IO_ID_OPEN;

    if (id_._type_ != geom_id::INVALID_TYPE) {
      out_ << id_._type_;
    } else {
      out_ << geom_id::IO_TYPE_INVALID;
    }

    out_ << geom_id::IO_ID_SEPARATOR;
    for (size_t i = 0; i < id_._addresses_.size (); i++) {
      if (id_._addresses_[i] == geom_id::INVALID_ADDRESS) {
        out_ << geom_id::IO_ADDRESS_INVALID;
      } else if (id_._addresses_[i] == geom_id::ANY_ADDRESS)
        {
          out_ << geom_id::IO_ADDRESS_ANY;
        } else {
        out_ << id_._addresses_[i];
      }
      if (i < id_._addresses_.size () - 1) {
        out_ << geom_id::IO_ADDRESS_SEPARATOR;
      }
    }
    out_ << geom_id::IO_ID_CLOSE;
    return out_;
  }

  std::istream & operator>> (std::istream & in_, geom_id & id_)
  {
    char open = in_.peek ();
    if (in_.eof ()) {
      return in_;
    }
    if (open != geom_id::IO_ID_OPEN) {
      in_.setstate (std::ios::failbit);
      return in_;
    } else {
      in_.get ();
    }

    char check = in_.peek ();
    if (in_.eof ()){
      in_.setstate (std::ios::failbit);
      return in_;
    }
    if (check != geom_id::IO_TYPE_INVALID) {
      in_ >> id_._type_;
      if (! in_) {
        return in_;
      }
    } else {
      id_._type_ = geom_id::INVALID_TYPE;
      in_.get ();
    }

    char separator = in_.peek ();
    if (in_.eof ())
      {
        in_.setstate (std::ios::failbit);
        return in_;
      }
    if (separator != geom_id::IO_ID_SEPARATOR)
      {
        in_.setstate (std::ios::failbit);
        return in_;
      }
    else
      {
        in_.get ();
      }
    int count = 0;
    id_._addresses_.clear ();
    id_._addresses_.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    while (true)
      {
        char local_check = in_.peek ();
        if (in_.eof ())
          {
            return in_;
          }
        uint32_t val;
        if (local_check == geom_id::IO_ADDRESS_INVALID)
          {
            id_.set_invalid (count);
            in_.get ();
          }
        else if (local_check == geom_id::IO_ADDRESS_ANY)
          {
            id_.set_any (count);
            in_.get ();
          }
        else
          {
            in_ >> val;
            if (! in_)
              {
                return in_;
              }
            id_.set (count, val);
          }

        char sep = in_.peek ();
        if (in_.eof ())
          {
            in_.setstate (std::ios::failbit);
            return in_;
          }
        if (sep == geom_id::IO_ID_CLOSE)
          {
            in_.get ();
            break;
          }
        else if (sep != geom_id::IO_ADDRESS_SEPARATOR)
          {
            in_.setstate (std::ios::failbit);
            return in_;
          }
        else
          {
            in_.get ();
          }
        count++;
        if (count == 100)
          {
            in_.setstate (std::ios::failbit);
            return in_;
          }
      }

    return in_;
  }

  bool geom_id::sub_id_comp (uint32_t si1_, uint32_t si2_)
  {
    return si1_ < si2_;
  }

  bool geom_id::match_impl (const geom_id & id_, bool exact_) const
  {
    return match (*this, id_, exact_);
  }

  bool geom_id::match (const geom_id & id_, bool exact_) const
  {
    return match_impl (id_, exact_);
  }

  bool geom_id::match (const geom_id & id1_, const geom_id & id2_, bool exact_)
  {
    if (exact_)
      {
        return id1_ == id2_;
      }
    if (id1_._type_ != id2_._type_)
      {
        return false;
      }
    if (id1_._addresses_.size () != id2_._addresses_.size ())
      {
        return false;
      }
    for (size_t i = 0; i < id1_._addresses_.size (); i++)
      {
        uint32_t addr1 = id1_._addresses_[i];
        uint32_t addr2 = id2_._addresses_[i];
        if (addr1 == INVALID_ADDRESS) return false;
        if (addr2 == INVALID_ADDRESS) return false;
        if (addr1 != addr2)
          {
            if (exact_)
              {
                return false;
              }
            if (addr1 == ANY_ADDRESS) continue;
            if (addr2 == ANY_ADDRESS) continue;
            return false;
          }
      }
    return true;
  }

  int geom_id::compare (const geom_id & id1_, const geom_id & id2_)
  {
    if (id1_._type_ < id2_._type_)
      {
        return -1;
      }
    else if (id1_._type_ > id2_._type_)
      {
        return +1;
      }
    else
      {
        if (lexicographical_compare(id1_._addresses_.begin (),
                                    id1_._addresses_.end (),
                                    id2_._addresses_.begin (),
                                    id2_._addresses_.end ()))
          {
            return -1;
          }
        else
          {
            if (lexicographical_compare(id2_._addresses_.begin (),
                                        id2_._addresses_.end (),
                                        id1_._addresses_.begin (),
                                        id1_._addresses_.end ()))
              {
                return +1;
              }
            else
              {
                return 0;
              }
          }
      }
  }

  bool geom_id::operator== (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) == 0;
  }

  bool geom_id::operator!= (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) != 0;
  }

  bool geom_id::operator< (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) < 0;
  }

  bool geom_id::operator<= (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) <= 0;
  }

  bool geom_id::operator>= (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) >= 0;
  }

  bool geom_id::operator> (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) > 0;
  }

  void geom_id::make (int type_, int depth_)
  {
    reset ();
    set_type (type_);
    set_depth (depth_);
    set_address (INVALID_ADDRESS);
    return;
  }

  // static
  void geom_id::make (geom_id & id_, int type_, int depth_)
  {
    id_.make (type_, depth_);
    return;
  }

} // end of namespace geomtools

// end of geom_id.cc
