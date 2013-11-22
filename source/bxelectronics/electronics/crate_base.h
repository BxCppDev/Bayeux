// -*- mode: c++; -*-
#ifndef ELECTRONICS_CRATE_BASE_H
#define ELECTRONICS_CRATE_BASE_H

#include <electronics/component_base.h>

namespace electronics {

  class crate_base : public component_base
  {
  public:

    /// Default constructor
    crate_base();

    /// Destructor
    virtual ~crate_base();

    /// Set the max number of modules
    void set_max_number_of_modules(uint32_t);

    /// Return the max number of modules
    uint32_t get_max_number_of_modules();

    /// Check if the crate has a known format
    bool has_format() const;

    /// Return the format
    const std::string & get_format() const;

    /// Set the format
    void set_format(const std::string &);

    /// Check if there is a module at given slot
    bool has_module(uint32_t slot_id_) const;

    /// Add a module
    void add_module(uint32_t slot_id_, component_handle_type & module_);

    /// Remove a module
    void remove_module(uint32_t slot_id_);

    /// Return a mutable handle to the embedded module given its slot id
    component_handle_type & grab_module(uint32_t slot_id_);

    /// Return a non mutable reference to a embedded module given its slot id
    const component_handle_type & get_module(uint32_t slot_id_) const;

    /* ***********
     * INTERFACE *
     *************/

    /// Initialize the board
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

    /// Common initialization of crate
    void _crate_initialize(const datatools::properties & config_,
                           component_pool_type& components_);

    /// Common termination of the crate
    void _crate_reset();

  private:

    std::string _format_; //!< Format of the crate
    uint32_t _max_number_of_modules_; //!< Maximum number of modules
    indexed_component_dict_type _modules_; //!< List of embedded modules

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(crate_base);

  };

} // end of namespace electronics

#endif // ELECTRONICS_CRATE_BASE_H
