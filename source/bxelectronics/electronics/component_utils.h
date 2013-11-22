// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_UTILS_H
#define ELECTRONICS_COMPONENT_UTILS_H

// Standard library
#include <string>
#include <map>

// Third party
// - Boost
#include <boost/cstdint.hpp>

// - Bayeux/datatools
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

namespace electronics {

  /// Build a name from a prefix and an integral index
  std::string make_indexed_name(const std::string & prefix_, uint32_t index_);

  class component_base;
  typedef datatools::handle<component_base> component_handle_type;

  class component_manager;

  //! \brief Internal entry for component objects stored in the component manager class
  class component_entry : public datatools::i_tree_dumpable  {
  public:

    //! \brief Status of the component
    enum status_type {
      STATUS_BLANK        = 0x0,
      STATUS_CREATED      = datatools::bit_mask::bit00,
      STATUS_INITIALIZED  = datatools::bit_mask::bit01,
    };

  public:

    /// Return the component name
    const std::string & get_component_name () const;

    /// Set the component name
    void set_component_name (const std::string &);

    /// Return the component ID
    const std::string & get_component_id () const;

    /// Set the component ID
    void set_component_id (const std::string &);

    /// Return a non mutable reference to the component configuration
    const datatools::properties & get_component_config () const;

    /// Return a mutable reference to the component configuration
    datatools::properties & grab_component_config ();

    /// Set the component configuration
    void set_component_config (const datatools::properties &);

    /// Check if there is the component manager
    bool has_component_manager() const;

    /// Install a handle to the component manager
    void set_component_manager(component_manager &);

    /// Reset the handle to the component manager
    void reset_component_manager();

    /// Default constructor
    component_entry();

    /// Destructor
    ~component_entry();

    /// Return the component status
    uint32_t get_component_status() const;

    /// Update the component status
    void update_component_status(uint32_t);

    /// Reset the component status
    void reset_component_status(uint32_t);

    /// Check if the component is created
    bool is_created() const;

    /// Check if the component is initialized
    bool is_initialized() const;

    /// Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherits_ = false) const;

    /// Return a non mutable handle to the component
    const component_handle_type & get_component_handle() const;

    /// Return a mutable handle to the component
    component_handle_type & grab_component_handle();

    /// Return a mutable handle to the initialized component
    component_handle_type & grab_initialized_component_handle ();

  private:

    std::string _component_name_;             //!< The name of the component
    std::string _component_id_;               //!< The ID (type) of the component
    datatools::properties _component_config_; //!< The configuration of the component
    uint32_t _component_status_;              //!< The status of the component
    component_handle_type _component_handle_; //!< The handle for the allocated component
    component_manager * _component_manager_;  //!< Handle to a component manager

  };

  /// Dictionary of component handles by label
  typedef std::map<std::string, component_handle_type> component_dict_type;
  /// Dictionary of component handles by index
  typedef std::map<uint32_t, component_handle_type> indexed_component_dict_type;
  /// Dictionary of component entries
  typedef std::map<std::string, component_entry> component_pool_type;

} // end of namespace electronics


#endif // ELECTRONICS_COMPONENT_UTILS_H
