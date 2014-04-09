// -*- mode: c++; -*-
#ifndef ELECTRONICS_BITSET_DESC_H
#define ELECTRONICS_BITSET_DESC_H

// Standard library
#include <string>

// Third party
// - Boost
#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/spirit/include/karma.hpp>
// - Bayeux/datatools
#include <datatools/enriched_base.h>

namespace electronics {

  /// \brief Description of the layout of a biset within a register
  /**
   *  A bitset as part of a register.
   *
   *              Register
   * :<----------------------------->:
   * :                               :
   * :        MSB            LSB     :
   * [x][x][x][0][0][0][1][0][1][x][x]
   *          :                :
   *          :<-------------->:
   *                Bitset
   */
  class bitset_desc : public datatools::enriched_base {

  public:

    static const uint16_t & invalid_lsb_position();

    /// Default constructor
    bitset_desc();

    /// Destructor
    virtual ~bitset_desc();

    /// Set the bitset size
    void set_size(uint16_t);

    /// Get the bitset size
    uint16_t get_size() const;

    /// Set the bitset LSB position
    void set_lsb_position(uint16_t);

    /// Get the bitset LSB position
    uint16_t get_lsb_position() const;

    /// Get the bitset MSB position
    uint16_t get_msb_position() const;

    /// Check the default value
    bool has_default_value() const;

    /// Set the bitset default value
    void set_default_value(const std::string &);

    /// Get the bitset default value
    const std::string & get_default_value() const;

    /// Set the bitset internals
    void set(uint16_t lsb_position_, uint16_t size_, const std::string &);

    /// Check initialization status
    bool is_initialized() const;

    /// Initialize
    void initialize(const datatools::properties& config_);

    /// Initialization
    void initialize();

    /// Reset
    void reset();

    /// Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

    /// Build a dynamic bitset from the bitset description
    void make(boost::dynamic_bitset<> & dbs_) const;

    /*
    /// Build a static bitset from the bitset description
    template<int N>
    void make(boost::bitset<N> & sbs_) const;
    */

    /// Convert to string
    bool to_string(std::string &) const;

    std::string to_string() const;

    /// Parse from a string
    bool from_string(const std::string &);

    /// Build a dynamic bitset mask from the bitset description
    void make_mask(uint16_t size_, boost::dynamic_bitset<> & dbs_) const;

    /// Print operator
    friend std::ostream & operator<<(std::ostream &, const bitset_desc&);

    /// Parse operator
    friend std::istream & operator>>(std::istream &, bitset_desc&);

  private:

    bool     _initialized_;      //!< Initialization flag
    uint16_t _size_;             //!< Size (in bits) of the bitset (8, 16, 24, 32, 48, 64, 128, 256)
    uint16_t _lsb_position_;     //!< LSB Position
    std::string _default_value_; //!< Default value string

  };

  /*
  template <typename Iter>
  struct bitset_desc_generator :
    boost::spirit::karma::grammar<Iter, bitset_desc()>
  {
    bitset_desc_generator() : bitset_desc_generator::base_type(start)
    {
      using boost::spirit::karma::char_;
      using boost::spirit::karma::ushort_;
      using boost::spirit::karma::string;
      start = '[' << string(get_name()) << ":" << ushort_(_size_) << ':' << ushort_(_lsb_position_) << -( ':' << string(_default_value_) ) << ']';
    }
    boost::spirit::karma::rule<Iter, bitset_desc()> start;
  };
  */

} // end of namespace electronics

#endif // ELECTRONICS_BITSET_DESC_H
