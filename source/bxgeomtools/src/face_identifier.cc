// face_identifier.cc

// Ourselves:
#include <geomtools/face_identifier.h>

// Standard library:
#include <bitset>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace geomtools {

  // static
  const uint32_t face_identifier::FACE_BITS_NONE;
  const uint32_t face_identifier::FACE_BITS_ANY;
  const uint32_t face_identifier::FACE_INDEX_NONE;
  const uint32_t face_identifier::FACE_INDEX_MIN;
  const uint32_t face_identifier::FACE_INDEX_MAX;
  const uint32_t face_identifier::FACE_INDEX_ANY;
  const uint32_t face_identifier::PART_INDEX_NONE;
  const uint32_t face_identifier::PART_INDEX_MIN;
  const uint32_t face_identifier::PART_INDEX_DEFAULT;
  const uint32_t face_identifier::PART_INDEX_MAX;
  const uint32_t face_identifier::PART_INDEX_ANY;

  void face_identifier::_set_defaults()
  {
    _mode_ = MODE_FACE_INVALID;
    _face_bits_ = FACE_BITS_NONE;
    _face_index_ = FACE_INDEX_NONE;
    return;
  }

  face_identifier::face_identifier()
  {
    _set_defaults();
    return;
  }

  face_identifier::face_identifier(uint32_t face_info_, mode_type mode_)
  {
    _set_defaults();
    if (mode_ == MODE_FACE_BITS) {
      set_face_bits(face_info_);
    } else if (mode_ == MODE_FACE_INDEX) {
      set_face_index(face_info_);
    }
    return;
  }

  face_identifier::mode_type face_identifier::get_mode() const
  {
    return _mode_;
  }

  bool face_identifier::has_mode() const
  {
    return _mode_ != MODE_FACE_INVALID;
  }

  bool face_identifier::is_unique() const
  {
    if (!has_mode()) {
      return false;
    }

    if (has_parts()) {
      for (unsigned int i = 0; i < _parts_.size(); i++) {
        if (is_invalid_part(i) || is_any_part(i)) {
          return false;
        }
      }
    }

    if (is_face_bits_mode()) {
      if (_face_bits_ == FACE_BITS_ANY || _face_bits_ == FACE_BITS_NONE) {
        return false;
      }
    } else if (is_face_index_mode()) {
      if (_face_index_ == FACE_INDEX_ANY || _face_index_ == FACE_INDEX_NONE) {
        return false;
      }
    }

    return true;
  }

  bool face_identifier::is_valid() const
  {
    if (!has_mode()) {
      return false;
    }
    if (is_face_bits_mode()) {
      if (_face_bits_ == FACE_BITS_NONE) {
        return false;
      }
    } else if (is_face_index_mode()) {
      if (_face_index_ == FACE_INDEX_NONE) {
        return false;
      }
    }

    if (has_parts()) {
      for (unsigned int i = 0; i < _parts_.size(); i++) {
        if (is_invalid_part(i)) {
          return false;
        }
      }
    }
    return true;
  }

  bool face_identifier::is_ok() const
  {
    return is_valid();
  }

  void face_identifier::reset()
  {
    invalidate();
    return;
  }

  void face_identifier::invalidate()
  {
    _parts_.clear();
    _set_defaults();
    return;
  }

  bool face_identifier::is_face_bits_mode() const
  {
    return _mode_ == MODE_FACE_BITS;
  }

  bool face_identifier::is_face_index_mode() const
  {
    return _mode_ == MODE_FACE_INDEX;
  }

  uint32_t face_identifier::get_parts_depth() const
  {
    return _parts_.size();
  }

  bool face_identifier::has_parts() const
  {
    return get_parts_depth() > 0;
  }

  bool face_identifier::has_part(unsigned int depth_) const
  {
    DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
    return _parts_[depth_] != PART_INDEX_NONE;
  }

  bool face_identifier::is_invalid_part(unsigned int depth_) const
  {
    DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
    return _parts_[depth_] == PART_INDEX_NONE;
  }

  bool face_identifier::match_part(unsigned int depth_, uint32_t part_) const
  {
    DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
    if (part_ == PART_INDEX_NONE) {
      return false;
    }
    if (_parts_[depth_] == part_ || _parts_[depth_] == PART_INDEX_ANY) {
      return true;
    }
    return false;
  }

  bool face_identifier::is_any_part(unsigned int depth_) const
  {
    DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
    return _parts_[depth_] == PART_INDEX_ANY;
  }

  void face_identifier::reset_part(unsigned int depth_)
  {
    DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
    _parts_[depth_] = PART_INDEX_NONE;
    return;
  }

  void face_identifier::set_any_part(unsigned int depth_)
  {
    DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
    _parts_[depth_] = PART_INDEX_ANY;
    return;
  }

  void face_identifier::set_any_parts()
  {
    for (unsigned int ipart = 0; ipart < _parts_.size(); ipart++) {
      _parts_[ipart] = PART_INDEX_ANY;
    }
    return;
  }

  uint32_t face_identifier::get_part(unsigned int depth_) const
  {
    DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
    return _parts_[depth_];
  }

  void face_identifier::set_part(unsigned int depth_, uint32_t part_)
  {
    if (part_ == PART_INDEX_NONE) {
      reset_part(depth_);
    } else if (part_ == PART_INDEX_ANY) {
      set_any_part(depth_);
    } else {
      DT_THROW_IF(depth_ >= _parts_.size(), std::range_error, "Invalid part depth [" << depth_ << "']!");
      DT_THROW_IF(part_ >= PART_INDEX_MAX, std::range_error,
                  "Part index overflow [" << part_ << "'] at depth [" << depth_ << "']!");
      _parts_[depth_] = part_;
    }
    return;
  }

  void face_identifier::prepend_part(uint32_t part_)
  {
    _parts_.insert(_parts_.begin(), part_);
    return;
  }

  void face_identifier::append_part(uint32_t part_)
  {
    _parts_.push_back(part_);
    return;
  }

  void face_identifier::append_parts(const std::vector<uint32_t> & parts_)
  {
    for (int i = 0; i < (int) parts_.size(); i++) {
      _parts_.push_back(parts_[i]);
    }
    return;
  }

  bool face_identifier::can_inherit_parts(unsigned int depth_) const
  {
    if (_parts_.size() >= depth_) {
      return true;
    }
    return false;
  }

  void face_identifier::inherit_parts(face_identifier & fid_, unsigned int depth_) const
  {
    DT_THROW_IF(!can_inherit_parts(depth_), std::logic_error,
                "Cannot inherit parts by depth=[" << depth_ << "]!");
    fid_ = *this;
    for (unsigned int i = 0; i < depth_; i++) {
      fid_._parts_.erase(fid_._parts_.begin());
    }
    return;
  }

  const std::vector<uint32_t> & face_identifier::get_parts() const
  {
    return _parts_;
  }

  void face_identifier::set_face_bits(uint32_t bits_)
  {
    // std::cerr << "DEVEL: face_identifier::set_face_bits: Entering." << std::endl;
    reset_face_index();
    _mode_ = MODE_FACE_BITS;
    _face_bits_ = bits_;
    // std::cerr << "DEVEL: face_identifier::set_face_bits: Exiting." << std::endl;
    return;
  }

  void face_identifier::reset_face_bits()
  {
    _face_bits_ = FACE_BITS_NONE;
    return;
  }

  void face_identifier::set_face_bit(uint32_t bit_)
  {
    // std::cerr << "DEVEL: face_identifier::set_face_bit: Entering." << std::endl;
    std::bitset<31> bs(bit_);
    DT_THROW_IF(bs.count() != 1, std::logic_error, "Not a single bit [" << bs << "]!");
    // std::cerr << "DEVEL: face_identifier::set_face_bit: Found a single active face bit." << std::endl;
    set_face_bits(bit_);
    // std::cerr << "DEVEL: face_identifier::set_face_bit: Exiting." << std::endl;
    return;
  }

  uint32_t face_identifier::get_face_bits() const
  {
    DT_THROW_IF(!is_face_bits_mode(), std::logic_error,
                "Face identifier does not use face bit mode!");
    return _face_bits_;
  }

  void face_identifier::set_face_bits_any()
  {
    set_face_bits(FACE_BITS_ANY);
    return;
  }

  bool face_identifier::is_face_bits_valid() const
  {
    return is_face_bits_mode() && _face_bits_ != FACE_BITS_NONE;
  }

  bool face_identifier::is_face_bits_any() const
  {
    return is_face_bits_mode() && _face_bits_ == FACE_BITS_ANY;
  }

  bool face_identifier::has_face_bits(uint32_t bits_) const
  {
    DT_THROW_IF(!is_face_bits_mode(), std::logic_error,
                "Face identifier does not use face bit mode!");
    return _face_bits_ & bits_;
  }

  bool face_identifier::has_face_bit(uint32_t bit_) const
  {
    DT_THROW_IF(!is_face_bits_mode(), std::logic_error,
                "Face identifier does not use face bit mode!");
    std::bitset<31> bs(bit_);
    DT_THROW_IF(bs.count() != 1, std::logic_error, "Not a single bit [" << bs << "]!");
    return _face_bits_ & bit_;
  }

  void face_identifier::set_face_index(uint32_t index_)
  {
    reset_face_bits();
    _mode_ = MODE_FACE_INDEX;
    switch (index_) {
    case FACE_INDEX_NONE:
    case FACE_INDEX_ANY:
      _face_index_ = index_;
      break;
    default:
      {
        DT_THROW_IF(index_ > FACE_INDEX_MAX, std::domain_error, "Invalid face index [" << index_ << "]!");
      }
    }
    _face_index_ = index_;
    return;
  }

  uint32_t face_identifier::get_face_index() const
  {
    DT_THROW_IF(!is_face_index_mode(), std::logic_error,
                "Face identifier does not use face index mode!");
    return _face_index_;
  }

  void face_identifier::reset_face_index()
  {
    _face_index_ = FACE_INDEX_NONE;
    return;
  }

  void face_identifier::set_face_index_any()
  {
    set_face_index(FACE_INDEX_ANY);
    return;
  }

  bool face_identifier::is_face_index_any() const
  {
    return is_face_index_mode() && _face_index_ == FACE_INDEX_ANY;
  }

  bool face_identifier::match(const face_identifier & fid_) const
  {
    if (!is_valid()) {
      return false;
    }
    if (!fid_.is_valid()) {
      return false;
    }
    if (fid_.has_parts()) {
      if (fid_.get_parts_depth() != this->get_parts_depth()) {
        return false;
      }
      for (unsigned int idepth = 0; idepth < fid_.get_parts_depth(); idepth++) {
        if (!this->match_part(idepth, fid_.get_part(idepth))) {
          return false;
        }
      }
    }
    if (fid_.is_face_bits_mode()) {
      return match_face_bits(fid_.get_face_bits());
    }
    if (fid_.is_face_index_mode()) {
      return match_face_index(fid_.get_face_index());
    }
    return false;
  }

  bool face_identifier::match_face_bits(uint32_t bits_) const
  {
    if (!is_face_bits_mode()) {
      return false;
    }
    if (!is_face_bits_valid()) {
      return false;
    }
    if (bits_ == FACE_BITS_NONE) {
      return false;
    }
    return _face_bits_ & bits_;
  }

  bool face_identifier::is_face_index_valid() const
  {
    return is_face_index_mode() && _face_index_ != FACE_INDEX_NONE;
  }

  bool face_identifier::match_face_index(uint32_t index_) const
  {
    if (!is_face_index_mode()) {
      return false;
    }
    if (!is_face_index_valid()) {
      return false;
    }
    if (index_ == FACE_INDEX_NONE) {
      return false;
    }
    return _face_index_ == index_ || _face_index_ == FACE_INDEX_ANY;
  }

  void face_identifier::to_string(std::string & word_) const
  {
    std::ostringstream oss;
    if (!is_valid()) {
      oss << "[!]";
    } else {
      oss << "[";
      if (has_parts()) {
        for (size_t ipart = 0; ipart < _parts_.size(); ipart++) {
          uint32_t part = _parts_[ipart];
          if (part == PART_INDEX_NONE) {
            oss << "!";
          } else if (part == PART_INDEX_ANY) {
            oss << "*";
          } else {
            oss << part;
          }
          if (ipart < _parts_.size() - 1) {
            oss << '.';
          }
        }
        oss << ':';
      }
      if (is_face_bits_mode()) {
        if (_face_bits_ == FACE_BITS_NONE) {
          oss << '!';
        } else if (_face_bits_ == FACE_BITS_ANY) {
          oss << '*';
        } else {
          std::bitset<31> bits(_face_bits_);
          oss << bits;
        }
      } else if (is_face_index_mode()) {
        if (_face_index_ == FACE_INDEX_NONE) {
          oss << '!';
        } else if (_face_index_ == FACE_INDEX_ANY) {
          oss << '*';
        } else {
          oss << _face_index_;
        }
      }
      oss << "]";
    }
    word_ = oss.str();
    return;
  }

  std::string face_identifier::to_string() const
  {
    std::string word;
    to_string(word);
    return word;
  }

  // static
  const face_identifier & face_identifier::face_bits_any()
  {
    static boost::scoped_ptr<face_identifier> _fid;
    if (!_fid) {
      _fid.reset(new face_identifier);
      _fid->set_face_bits_any();
    }
    return *_fid;
  }

  // static
  const face_identifier & face_identifier::face_index_any()
  {
    static boost::scoped_ptr<face_identifier> _fid;
    if (!_fid) {
      _fid.reset(new face_identifier);
      _fid->set_face_index_any();
    }
    return *_fid;
  }

  // static
  const face_identifier & face_identifier::face_invalid()
  {
    static boost::scoped_ptr<face_identifier> _fid;
    if (!_fid) {
      _fid.reset(new face_identifier);
      _fid->invalidate();
    }
    return *_fid;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_,
                            const face_identifier & face_id_)
  {
    out_ << face_id_.to_string();
    return out_;
  }

} // end of namespace geomtools
