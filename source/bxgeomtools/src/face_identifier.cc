// face_identifier.cc

// Ourselves:
#include <geomtools/face_identifier.h>

// Standard library:
#include <bitset>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/lexical_cast.hpp>

// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/ioutils.h>

namespace geomtools {

  namespace parsing {

    struct face_identifier_repr
    {
      std::vector<std::string> parts;
      std::string mode;
      std::string face_repr;

      void to_face_identifier(face_identifier &) const;
    };

    void face_identifier_repr::to_face_identifier(face_identifier & fi_) const
    {
      // std::cerr << "DEBUG: Representation:\n";
      // std::cerr << "DEBUG:   mode = '" << this->mode << "'\n";
      // std::cerr << "DEBUG:   face_repr = '" << this->face_repr << "'\n";
      // for (auto part : this->parts) {
      //   std::cerr << "DEBUG:   part = '" << part << "'\n";
      // }
      if (this->mode == "invalid") {
      } else {
        if (this->mode == "bits") {
          DT_THROW_IF(this->face_repr.length() > 31 || this->face_repr.length() < 1,
                      std::range_error,
                      "Invalid face bits representation (" << this->face_repr << ")");
          std::bitset<31> bs(this->face_repr);
          fi_.set_face_bits((uint32_t) bs.to_ulong());
        } else if (this->mode == "index") {
          uint32_t index = face_identifier::FACE_INDEX_NONE;
          if (this->face_repr == "!") {
            index = face_identifier::FACE_INDEX_NONE;
          } else if (this->face_repr == "*") {
            index = face_identifier::FACE_INDEX_ANY;
          } else {
            // std::cerr << "DEBUG: casting face index '" << this->face_repr << "'...\n";
            index = boost::lexical_cast<uint32_t>(this->face_repr);
          }
          fi_.set_face_index(index);
        } else {
          DT_THROW(std::logic_error, "Unknown mode '" << this->mode << "'!");
        }
        for (const std::string & part_repr : this->parts) {
          uint32_t part = face_identifier::PART_INDEX_NONE;
          if (part_repr == "!") {
            // std::cerr << "DEBUG: found NONE part number\n";
            part = face_identifier::PART_INDEX_NONE;
          } else if (part_repr == "*") {
            // std::cerr << "DEBUG: found ANY part number\n";
            part = face_identifier::PART_INDEX_ANY;
          } else {
            // std::cerr << "DEBUG: casting part number '" << part_repr << "'...\n";
            part = boost::lexical_cast<uint32_t>(part_repr);
          }
          fi_.append_part(part);
        }
      }

      return;
    }

  } // end of namespace parsing

} // end of namespace geomtools

BOOST_FUSION_ADAPT_STRUCT(geomtools::parsing::face_identifier_repr,
                          (std::vector<std::string>, parts)
                          (std::string, mode)
                          (std::string, face_repr)
                          )

namespace geomtools {

  namespace parsing {

    namespace qi = boost::spirit::qi;

    /// \brief Gramme for a face_identifier
    template <typename Iterator>
    struct face_identifier_grammar
      : boost::spirit::qi::grammar<Iterator,
                                   face_identifier_repr()>
    {
      face_identifier_grammar()
        : face_identifier_grammar::base_type(start)
      {
        using boost::phoenix::at_c;

        // "0", "1"... "9"  or "1..0", "1..9"
        gaddress =
          qi::string("0") |
          (
           qi::char_("1-9") >> *qi::char_("0-9")
           );

        // "!", "*"... "12"
        gpart = qi::string("!") | qi::string("*") | gaddress;

        // "1.0", "12.!.42", "*.12.42"
        gparts = ( gpart % '.');

        // "0" or "1"
        gbit = qi::char_("01");

        // "0", "1", "10", ... "01001"
        gbits = +gbit;

        // "!", "*"... "01001"
        gindex = qi::string("!") | qi::string("*") | gaddress;

        // Rule definitions:
        start = ( qi::lit("[")
                  >> (qi::string("!")           [at_c<1>(qi::_val) = "invalid"]
                      | (
                         -( qi::lit("parts=")
                            >> gparts              [at_c<0>(qi::_val) = qi::_1]
                            >> qi::lit(":")
                            )
                         >> (
                             (qi::string("bits=")  [at_c<1>(qi::_val) = "bits"]
                              >> gbits             [at_c<2>(qi::_val) = qi::_1]
                              )
                             |
                             (qi::string("index=") [at_c<1>(qi::_val) = "index"]
                              >> gindex            [at_c<2>(qi::_val) = qi::_1]
                              )
                             )
                         )
                      )
                  >> qi::lit("]")
                  );

        return;
      }

      // Rules:
      qi::rule<Iterator, face_identifier_repr()> start;
      qi::rule<Iterator, std::string()> gpart;
      qi::rule<Iterator, std::vector<std::string>()> gparts;
      qi::rule<Iterator, char()>        gbit;
      qi::rule<Iterator, std::string()> gbits;
      qi::rule<Iterator, std::string()> gaddress;
      qi::rule<Iterator, std::string()> gindex;

    };

  } // end of namespace parsing

} // end of namespace geomtools

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

  bool face_identifier::is_many() const
  {
    if (!has_mode()) {
      return false;
    }

    bool many_faces = false;
    if (is_face_bits_mode()) {
      //std::cerr << "DEVEL: is_face_bits_mode\n";
      if (_face_bits_ == FACE_BITS_NONE) {
        //std::cerr << "DEVEL: FACE_BITS_NONE\n";
        return false;
      } else if (_face_bits_ != FACE_BITS_ANY) {
        std::bitset<31> bs(_face_bits_);
        //std::cerr << "DEVEL: !FACE_BITS_ANY: bs = " << bs.to_string() << "\n";
        //std::cerr << "DEVEL:   bs.count = " << bs.count() << "\n";
        if (bs.count() > 1) many_faces = true;
      }
    } else if (is_face_index_mode()) {
      //std::cerr << "DEVEL: is_face_index_mode\n";
      if (_face_index_ == FACE_INDEX_NONE) {
        //std::cerr << "DEVEL: FACE_INDEX_NONE\n";
        return false;
      } else if (_face_index_ != FACE_INDEX_ANY) {
        //std::cerr << "DEVEL: !FACE_INDEX_ANY" << "\n";
      } else {
        many_faces = true;
      }
    }

    bool many_parts = false;
    if (has_parts()) {
      //std::cerr << "DEVEL: has_parts" << "\n";
      for (unsigned int i = 0; i < _parts_.size(); i++) {
        if (is_invalid_part(i)) {
          return false;
        } else {
          if (is_any_part(i)) {
            many_parts = true;
          }
        }
      }
    }

    return many_faces || many_parts;
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
      std::bitset<31> bs(_face_bits_);
      if (bs.count() != 1) return false;
    } else if (is_face_index_mode()) {
      if (_face_index_ == FACE_INDEX_ANY || _face_index_ == FACE_INDEX_NONE) {
        return false;
      }
    }

    return true;
  }

  bool face_identifier::is_none() const
  {
    if (!is_valid()) {
      return true;
    }
    if (is_face_bits_mode()) {
      if (_face_bits_ == FACE_BITS_NONE) {
        return true;
      }
    } else if (is_face_index_mode()) {
      if (_face_index_ == FACE_INDEX_NONE) {
        return true;
      }
    }
    if (has_parts()) {
      for (unsigned int i = 0; i < _parts_.size(); i++) {
        if (is_invalid_part(i)) {
          return true;
        }
      }
    }
    return false;
  }

  bool face_identifier::is_valid() const
  {
    if (!has_mode()) {
      return false;
    }
    /*
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
    */
    return true;
  }

  bool face_identifier::is_ok() const
  {
    // equivalent to: is_valid() && !is_none();
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
    reset_face_index();
    _mode_ = MODE_FACE_BITS;
    _face_bits_ = bits_;
    return;
  }

  void face_identifier::reset_face_bits()
  {
    _face_bits_ = FACE_BITS_NONE;
    return;
  }

  void face_identifier::set_face_bit(uint32_t bit_)
  {
    std::bitset<31> bs(bit_);
    DT_THROW_IF(bs.count() != 1, std::logic_error, "Not a single bit [" << bs << "]!");
    set_face_bits(bit_);
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

  bool face_identifier::parse(const std::string & from_)
  {
    this->invalidate();
    namespace qi = boost::spirit::qi;
    try {
      std::string::const_iterator str_iter = from_.begin();
      std::string::const_iterator end_iter = from_.end();
      geomtools::parsing::face_identifier_grammar<std::string::const_iterator> fig;
      geomtools::parsing::face_identifier_repr repr;
      bool res = boost::spirit::qi::phrase_parse(str_iter,
                                                 end_iter,
                                                 // Begin grammar:
                                                 (
                                                  fig[boost::phoenix::ref(repr) = boost::spirit::qi::_1]
                                                  )
                                                 // End grammar
                                                 ,
                                               boost::spirit::ascii::space);
      if (!res || str_iter != end_iter) {
        // std::cerr << "ERROR: parse error at '" << *str_iter << "'!\n";
        return false;
      } else {
        repr.to_face_identifier(*this);
      }
    } catch (std::exception & error) {
      // std::cerr << "ERROR: parse error : " << error.what() << "\n";
      this->invalidate();
      return false;
    }
    return true;
  }

  void face_identifier::to_string(std::string & word_) const
  {
    std::ostringstream oss;
    if (!is_valid()) {
      oss << "[!]";
    } else {
      oss << "[";
      if (has_parts()) {
        oss << "part=";
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
        oss << "bits=";
        if (_face_bits_ == FACE_BITS_NONE) {
          oss << '!';
        } else if (_face_bits_ == FACE_BITS_ANY) {
          oss << '*';
        } else {
          // std::bitset<31> bits(_face_bits_);
          // oss << bits;
          oss << datatools::io::to_binary_2(_face_bits_);
        }
      } else if (is_face_index_mode()) {
        oss << "index=";
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

  void face_identifier::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Mode : ";
    if (_mode_ == MODE_FACE_INVALID) {
      out_ << "<invalid>";
    } else if (_mode_ == MODE_FACE_BITS) {
      out_ << "'bits'";
    } else if (_mode_ == MODE_FACE_INDEX) {
      out_ << "'index'";
    } else {
      out_ << "<any>";
    }
    out_ << std::endl;

    if (_mode_ == MODE_FACE_BITS || _mode_ == MODE_FACE_ANY) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Face bits : ";
      if (_face_bits_ == FACE_BITS_NONE) {
        out_ << "<none>";
      } else if (_face_bits_ == FACE_BITS_ANY) {
        out_ << "<any>";
      } else {
        std::bitset<31> bs(_face_bits_);
        out_ << bs.to_string();
      }
      out_ << std::endl;
    }

    if (_mode_ == MODE_FACE_INDEX || _mode_ == MODE_FACE_ANY) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Face index : ";
      if (_face_index_ == FACE_INDEX_ANY) {
        out_ << "<any>";
      } else if (_face_index_ == FACE_INDEX_NONE) {
        out_ << "<none>";
      } else {
        out_ << _face_index_;
      }
      out_ << std::endl;
    }

    if (has_parts()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Parts : ";
      out_ << "<depth=" << _parts_.size() << ">";
      out_ << std::endl;
      for (std::size_t ip = 0; ip < _parts_.size(); ip++) {
        out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
        if (ip + 1 == _parts_.size()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        out_ << "Part at depth [" << ip << "] = ";
        uint32_t part = _parts_[ip];
        if (part == PART_INDEX_ANY) {
          out_ << "<any>";
        } else if (part == PART_INDEX_NONE) {
          out_ << "<none>";
        } else {
          out_ << part;
        }
        out_ << std::endl;
      }
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Valid  : " << (is_valid()? "<yes>": "<no>") << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "None   : " << (is_none()? "<yes>": "<no>") << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Unique : " << (is_unique()? "<yes>": "<no>") << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Many   : " << (is_many()? "<yes>": "<no>") << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Ok     : " << (is_ok()? "<yes>": "<no>") << std::endl;

    return;
  }

} // end of namespace geomtools
