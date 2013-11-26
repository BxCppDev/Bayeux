// -*- mode: c++; -*-
#ifndef ELECTRONICS_RACK_BASE_H
#define ELECTRONICS_RACK_BASE_H

#include <electronics/component_base.h>

namespace electronics {

  /// \brief The base class of an electronic rack
  class rack_base : public component_base
  {
  public:

    /// Default constructor
    rack_base();

    /// Destructor
    virtual ~rack_base();

    /// Set the max number of crates
    void set_max_number_of_crates(uint32_t);

    /// Return the max number of crates
    uint32_t get_max_number_of_crates();

    /// Check if the rack has a known format
    bool has_format() const;

    /// Return the format
    const std::string & get_format() const;

    /// Set the format
    void set_format(const std::string &);

    /// Check if there is a crate at given slot
    bool has_crate(uint32_t slot_id_) const;

    /// Add a crate
    void add_crate(uint32_t slot_id_, component_handle_type & board_);

    /// Remove a crate
    void remove_crate(uint32_t slot_id_);

    /// Return a mutable handle to the embedded crate
    component_handle_type & grab_crate(uint32_t slot_id_);

    /// Return a non mutable reference to a embedded crate given its name
    const component_handle_type & get_crate(uint32_t slot_id_) const;

    /* ***********
     * INTERFACE *
     *************/

    /// Initialize the rack
    virtual void initialize(const datatools::properties& config_,
                            component_pool_type& components_);

    /// Reset the board
    virtual void reset();

    // Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

  protected:

    /// Common initialization of rack
    void _rack_initialize(const datatools::properties & config_,
                          component_pool_type& components_);

    /// Common termination of the rack
    void _rack_reset();

  private:

    std::string _format_; //!< Format of the rack
    uint32_t _max_number_of_crates_; //!< Maximum number of crates
    indexed_component_dict_type _crates_; //!< List of embedded crates

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(rack_base);

  };

} // end of namespace electronics

#endif // ELECTRONICS_RACK_BASE_H
