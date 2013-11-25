// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_BASE_H
#define ELECTRONICS_COMPONENT_BASE_H

// Standard library
#include <string>
#include <vector>

// Third party
// - Boost
#include <boost/cstdint.hpp>

// - Bayeux/datatools
#include <datatools/factory_macros.h>
#include <datatools/advanced_object.h>

// This project
#include <electronics/component_utils.h>
#include <electronics/component_types.h>
#include <electronics/component_macros.h>

namespace electronics {

  class component_manager;

  /// \brief The base class of all electronic component models
  class component_base : public datatools::advanced_object
  {
  public:

    /// Default constructor
    component_base();

    /// Default constructor
    explicit component_base(component_type type_);

    /// Destructor
    virtual ~component_base();

    /// Check if component has a type
    bool has_type() const;

    /// Set the type of the electronic component
    void set_type(component_type type_);

    /// Return the type of the electronic component
    component_type get_type() const;

    /// Build the list of all embedded components' labels
    void build_embedded_labels(std::vector<std::string>&) const;

    /// Check if there is a embedded component with a given label
    bool has_embedded(const std::string & embedded_label_) const;

    /// Add a embedded component
    void add_embedded(const std::string & embedded_label_,
                      component_handle_type & embedded_);

    /// Remove a embedded component
    void remove_embedded(const std::string & embedded_label_);

    /// Return a mutable handle to the embedded component
    component_handle_type & grab_embedded(const std::string & embedded_label_);

    /// Return a non mutable handle to the embedded component
    const component_handle_type & get_embedded(const std::string & embedded_label_) const;

    /* ***********
     * INTERFACE *
     *************/

    /// Check if component is initialized
    bool is_initialized() const;

    /// Initialize the component
    void initialize_simple();

    /// Initialize the component using only a list of properties without the needs of other components
    void initialize_standalone(const datatools::properties& config_);

    /// Initialize the component using a list of properties with access to a dictionary of other components
    virtual void initialize(const datatools::properties& config_,
                            component_pool_type& components_);

    /// Reset the component
    virtual void reset();

    // Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

    /// Allow embedded components (default implementation)
    virtual bool allow_embedded_components() const;

  protected:

    /// Basic initialization of the component
    void _component_initialize(const datatools::properties & config_,
                               component_pool_type& components_);

    /// Common termination of the component
    void _component_reset();

    /// Check initialization
    bool _is_initialized() const;

    /// Set initialization flag
    void _set_initialized(bool);

  private:

    bool                  _initialized_; //!< Initialization flag
    int32_t               _type_;        //!< Type of the electronics component model
    component_dict_type   _embedded_components_; //!< Dictionary of embedded components models

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(component_base);

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(component_base);

  };

} // end of namespace electronics

#endif // FLELECTRONICS_COMPONENT_BASE_H
