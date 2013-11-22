// -*- mode: c++; -*-
#ifndef ELECTRONICS_BITSET_DESC_H
#define ELECTRONICS_BITSET_DESC_H

// Standard library
#include <string>

// Third party
// - Boost
#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>
// - Bayeux/datatools
#include <datatools/advanced_object.h>

namespace electronics {

  class bitset_desc : public datatools::advanced_object {

  public:

    /// Default constructor
    bitset_desc();

    /// Destructor
    virtual ~bitset_desc();

    /// Check validity
    bool is_valid() const;

    /// Set the bitset size
    void set_size(uint8_t);

    /// Get the bitset size
    uint8_t get_size() const;

    /// Set the bitset LSB position
    void set_lsb_position(uint8_t);

    /// Get the bitset LSB position
    uint8_t get_lsb_position() const;

    /// Get the bitset MSB position
    uint8_t get_msb_position() const;

    /// Set the bitset default value
    void set_default_value(const std::string &);

    /// Get the bitset default value
    const std::string & get_default_value() const;

    /// Initialize
    void initialize(const datatools::properties& config_);

    /// Reset
    void reset();

    /// Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

    /// Build a dynamic bitset from the bitset description
    void make(boost::dynamic_bitset<> & dbs_) const;

    /// Build a dynamic bitset mask from the bitset description
    void make_mask(uint8_t size_, boost::dynamic_bitset<> & dbs_) const;

  private:

    uint8_t _size_;         //!< Size (in bits) of the bitset (8, 16, 24, 32, 48, 64)
    uint8_t _lsb_position_; //!< LSB Position
    std::string _default_value_; //!< Default value string

  };

} // end of namespace electronics

#endif // ELECTRONICS_BITSET_DESC_H
