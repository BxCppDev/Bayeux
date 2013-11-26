// -*- mode: c++; -*-
#ifndef ELECTRONICS_REGISTER_MODEL_BASE_H
#define ELECTRONICS_REGISTER_MODEL_BASE_H

// Standard library
#include <string>
#include <map>

// Third party
// - Boost
#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>

// This project
#include <electronics/component_model_base.h>
#include <electronics/bitset_desc.h>

namespace electronics {

  /// \brief Description of the layout of a register of finite size
  class register_model_base : public component_model_base
  {
  public:
    typedef std::map<std::string, bitset_desc> bitsets_dict_type;

    enum size_type {
      SIZE_UNDEFINED = 0,
      SIZE_8  = 8,
      SIZE_16 = 16,
      SIZE_24 = 24,
      SIZE_32 = 32,
      SIZE_48 = 48,
      SIZE_64 = 64
    };

    /// Default constructor
    register_model_base();

    /// Destructor
    virtual ~register_model_base();

    /// Set read access flag
    void set_readable(bool);

    /// Check read access flag
    bool is_readable() const;

    /// Set write access flag
    void set_writable(bool);

    /// Check write access flag
    bool is_writable() const;

    /// Set the size
    void set_size(uint8_t);

    /// Get the size
    uint8_t get_size() const;

    /// Get the effective size
    uint8_t get_effective_size() const;

    /// Get the register mask
    const boost::dynamic_bitset<> & get_mask() const;

    /// Add a bitset
    void add_bitset(const std::string & name_, const bitset_desc &);

    /// Add a bitset a some given LSB position
    void add_bitset(const std::string & name_,
                    uint8_t lsb_position_,
                    uint8_t size_,
                    const std::string & default_value_ = "");

    /// Append a bitset from the first available bit slot found in the mask
    void append_bitset(const std::string & name_,
                       uint8_t size_,
                       const std::string & default_value_ = "");

    /// Build a dynamic bitset from the register description
    void make(boost::dynamic_bitset<> & dbs_) const;

    /* ***********
     * INTERFACE *
     *************/

    /// Initialize the register layout
    virtual void initialize(const datatools::properties& config_,
                            component_pool_type& components_);

    /// Reset the register layout
    virtual void reset();

    // Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

    /// Allow embedded components
    virtual bool allow_embedded_components() const;

  protected:

    /// Common initialization of the board
    void _register_initialize(const datatools::properties & config_,
                              component_pool_type& components_);

    /// Common termination of the board
    void _register_reset();

  private:

    uint8_t _size_; //!< Size of the register
    boost::dynamic_bitset<> _mask_; //!< Full bit mask
    bool _readable_; //!< Read access flag
    bool _writable_; //!< Write access flag
    // uint64_t _address_; //!< Hardware address
    bitsets_dict_type _bitsets_; //!< Dictionary of bitsets

    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(register_model_base);

  };

} // end of namespace electronics

#endif // ELECTRONICS_REGISTER_MODEL_BASE_H
