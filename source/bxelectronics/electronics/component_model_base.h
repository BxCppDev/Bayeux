// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_MODEL_BASE_H
#define ELECTRONICS_COMPONENT_MODEL_BASE_H

// Standard library
#include <string>
#include <vector>

// Third party
// - Boost
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>

// - Bayeux/datatools
#include <datatools/factory_macros.h>
#include <datatools/enriched_base.h>

// This project
#include <electronics/component_utils.h>
#include <electronics/component_types.h>
#include <electronics/component_macros.h>
#include <electronics/logical_component.h>
#include <electronics/digital_port_desc.h>
#include <electronics/digital_channel_desc.h>
#include <electronics/register_desc.h>

namespace electronics {

  class component_manager;
  class logical_component;

  /// \brief The base class of all electronic component models
  /**
   *                                          +--------+
   *                                          |        |
   *  Electronic component model (base):      |        v
   *  +---------------------------------------|-----------------------------+
   *  |                                       |                             |
   *  |  +-------------------------+          |                             |
   *  |  | Logical component       |----------+                             |
   *  |  |                         |  [model]                               |
   *  |  |                         |                                        |
   *  |  |                         |                                        |
   *  |  +-------------------------+                                        |
   *  |       ^       ^        ^                                            |
   *  |       |        \        \                                           |
   *  |   [mother]  [mother]  [mother]                                      |
   *  |       |          \        \                                         |
   *  |  +----------+  +------+  +--------------+ Dictionary                |
   *  |  | "A"      |  | "B"  |  | "C"          | of                        |
   *  |  |----------|  |------|  |--------------| embedded                  |
   *  |  |          |  |      |  |              | physical                  |
   *  |  |          |  |      |  |              | components                |
   *  |  +----------+  +------+  +--------------+                           |
   *  |       |            \              \                                 |
   *  +-------|-------------\--------------\--------------------------------+
   *          |              \              \
   *      [logical]       [logical]       [logical]
   *          |                \              \
   *  +-------|-------+  +------\--------+  +--\------------+
   *  |       v       |  |       v       |  |   v           |
   *  | +-----------+ |  | +-----------+ |  | +-----------+ |
   *  | |           | |  | |           | |  | |           | |
   *  | |           | |  | |           | |  | |           | |
   *  | |           | |  | |           | |  | |           | |
   *  | +-----------+ |  | +-----------+ |  | +-----------+ |
   *  +---------------+  +---------------+  +---------------+
   *
   */
  class component_model_base : public datatools::enriched_base
  {
  public:

    friend class component_manager;

    /// Dictionary of physical components
    typedef std::map<std::string, physical_component>   physical_dict_type;
    /// Dictionary of digital ports
    typedef std::map<std::string, digital_port_desc>    digital_ports_dict_type;
    /// Dictionary of digital channels
    typedef std::map<std::string, digital_channel_desc> digital_channels_dict_type;
    /// Dictionary of registers
    typedef std::map<std::string, register_desc>        registers_dict_type;

    /// Default constructor
    component_model_base();

    /// Constructor
    explicit component_model_base(component_type type_);

    /// Destructor
    virtual ~component_model_base();

    /// Check if component has a type
    bool has_type() const;

    /// Set the type of the electronic component
    void set_type(component_type type_);

    /// Return the type of the electronic component
    component_type get_type() const;

    /// Allow embedded components (default implementation)
    bool is_allow_embedded_components() const;

    /// Set the allow embedded components flag
    void set_allow_embedded_components(bool);

    /// Check if there is an embedded component with a given label
    bool has_embedded_component(const std::string & component_label_) const;

    /// Add an embedded component
    physical_component & add_embedded_component(const std::string & component_label_,
                                                const component_model_base & component_model_);

    /// Remove an embedded component
    void remove_embedded_component(const std::string & component_label_);

    /// Return a non mutable reference to an embedded component by label
    const physical_component & get_embedded_component(const std::string & component_label_) const;

    /// Add a digital port
    void add_digital_port(const std::string & name_, const digital_port_desc &);

    /// Add a digital channel
    void add_digital_channel(const std::string & name_, const digital_channel_desc &);

    /// Add a register
    void add_register(const std::string & name_, const register_desc &);

    /// Return a non mutable reference to the list of digital ports
    const digital_ports_dict_type & get_digital_ports() const;

    /// Return a non mutable reference to the list of digital channels
    const digital_channels_dict_type get_digital_channels() const;

    /* ***********
     * INTERFACE *
     *************/

    /// Check if component model is initialized
    bool is_initialized() const;

    /// Initialize the component model
    void initialize_simple();

    /// Initialize the component model using only a list of properties without the needs of other component models
    void initialize_standalone(const datatools::properties& config_);

    /// Initialize the component model using a list of properties with access to a dictionary of other component models
    void initialize(const datatools::properties& config_,
                    component_model_pool_type& component_models_);

    /// Reset the component model
    void reset();

    // Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

    /// Check the logical
    bool has_logical() const;

    /// Get a non mutable reference to the embedded logical component
    const logical_component & get_logical() const;

  protected:

    virtual bool _check_embedded_type(component_type) const;

    /// Get a mutable reference to the embedded logical component
    logical_component & _grab_logical();

    /// Add an embedded component
    physical_component & _add_embedded_physical(const std::string &,
                                                const logical_component &);

    /// Remove an embedded component
    void _remove_embedded_physical(const std::string &);

    /// Overloaded initialization method
    virtual void _at_initialize(const datatools::properties & config_,
                                component_model_pool_type& components_);

    /// Overloaded reset method
    virtual void _at_reset();

    /// Post add
    virtual void _post_add_embedded_physical(const std::string & embedded_label_);

    /// Post remove
    virtual void _post_remove_embedded_physical(const std::string & embedded_label_);

    /// Basic initialization of the component model
    void _component_initialize(const datatools::properties & config_,
                               component_model_pool_type& components_);

    /// Common termination of the component model
    void _component_reset();

  private:

    /// Set initialization flag
    void _set_initialized_(bool);

    /// Instantiate the logical
    void _create_logical_();

  private:

    int32_t                    _type_;                         //!< Device type of the electronics component model
    bool                       _allow_embedded_components_;    //!< Flag to allow embedded components
    physical_dict_type         _embedded_physical_components_; //!< Hardcoded physical components
    digital_ports_dict_type    _digital_ports_;     //!< Dictionary of digital ports
    digital_channels_dict_type _digital_channels_;  //!< Dictionary of digital channels
    registers_dict_type        _registers_;         //!< Dictionary of registers
    bool                       _initialized_;       //!< Initialization flag
    boost::scoped_ptr<logical_component> _logical_; //!< Scoped pointer to the logical component associated to the component model

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(component_model_base);

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(component_model_base);

  };

} // end of namespace electronics

#endif // FLELECTRONICS_COMPONENT_MODEL_BASE_H
