// -*- mode: c++; -*-
#ifndef ELECTRONICS_RACK_MODEL_BASE_H
#define ELECTRONICS_RACK_MODEL_BASE_H

#include <electronics/component_model_base.h>

namespace electronics {

  class physical_component;

  /// \brief The base class of an electronic rack
  /**
   *     Rack layout
   *     +----------------+
   *     |                |
   *     |     Crate      | Slot 3
   *     |                |
   *     +----------------+
   *     |                |
   *     |     Crate      | Slot 2
   *     |                |
   *     +----------------+
   *     |                |
   *     |     Crate      | Slot 1
   *     |                |
   *     +----------------+
   *     |                |
   *     |     Crate      | Slot 0
   *     |                |
   *     +----------------+
   *
   */
  class rack_model_base : public component_model_base
  {
  public:

    /// Default constructor
    rack_model_base();

    /// Destructor
    virtual ~rack_model_base();

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
    bool has_crate(uint32_t crate_slot_) const;

    /// Add a crate
    physical_component &
    add_crate(uint32_t crate_slot_,
              const component_model_base & crate_model_,
              const std::string & crate_label_ = "");

    /// Remove a crate
    void remove_crate(uint32_t crate_slot_);

    /// Return a non mutable reference to an embedded crate by slot
    const physical_component & get_crate(uint32_t crate_slot_);

    /// Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

  protected:

    /// Overloaded initialization method
    virtual void _at_initialize(const datatools::properties & config_,
                                component_model_pool_type& components_);

    /// Overloaded reset method
    virtual void _at_reset();

    /// Ensure explicit unregistration of a crate if needed
    virtual void _post_remove_embedded_physical(const std::string & embedded_label_);

    /// Common initialization of rack
    void _rack_initialize(const datatools::properties & config_,
                          component_model_pool_type& components_);

    /// Common termination of the rack
    void _rack_reset();

  private:

    std::string _format_; //!< Format of the rack
    uint32_t    _max_number_of_crates_; //!< Maximum number of crates
    indexed_labels_dict_type _crate_labels_; //!< List of crate labels by slot index

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(rack_model_base);

  };

} // end of namespace electronics

#endif // ELECTRONICS_RACK_MODEL_BASE_H
