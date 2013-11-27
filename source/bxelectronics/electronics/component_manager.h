// -*- mode: c++; -*-
/* component_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-11-12
 * Last modified : 2013-11-13
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   An electronic component manager.
 *
 * History:
 *
 */
#ifndef ELECTRONICS_COMPONENT_MANAGER_H_
#define ELECTRONICS_COMPONENT_MANAGER_H_

// Standard Library
#include <string>
#include <sstream>
#include <typeinfo>
#include <stdexcept>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// Third Party
// - Bayeux/datatools
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/bit_mask.h>
// - Bayeux/geomtools
#include <geomtools/id_mgr.h>

// This project
#include <electronics/component_utils.h>
#include <electronics/component_model_base.h>
#include <electronics/mapping.h>

namespace datatools {

  class properties;
  class multi_properties;

}

namespace electronics {

  /*! \brief Electronics component management class
   *
   */
  class component_manager : public datatools::i_tree_dumpable {

  public:

    static const std::string & default_top_level_name();

    enum flag_type {
      BLANK              = 0,
      NO_PRELOAD         = datatools::bit_mask::bit00,
      FORCE_INITIALIZATION_AT_LOAD = datatools::bit_mask::bit01,
      FACTORY_VERBOSE    = datatools::bit_mask::bit02,
      NO_BUILD_MAPPING   = datatools::bit_mask::bit03
    };

    /// Constructor
    component_manager(uint32_t flag = 0);

    /// Destructor
    ~component_manager();

    /// Set the name of the setup
    void set_setup_label(const std::string& name);

    /// Set the description of the setup
    void set_setup_description(const std::string& description);

    /// Set the version of the setup
    void set_setup_version(const std::string& version);

    /// Get the name of the setup
    const std::string& get_setup_label() const;

    /// Get the description of the setup
    const std::string& get_setup_description() const;

    /// Get the version of the setup
    const std::string& get_setup_version() const;

    /// Set the factory preload flag
    void set_preload(bool);

    /// Get the factory preload flag
    bool get_preload() const;

    /// Set the build mapping flag
    void set_mapping_requested(bool);

    /// Get the build mapping flag
    bool get_mapping_requested() const;

    /// Set the 'force initialization at load' flag
    void set_force_initialization_at_load(bool);

    /// Get the 'force initialization at load' flag
    bool get_force_initialization_at_load() const;

    /// Add a property prefix to be preserved in models
    void add_auxiliary_property_prefix(const std::string & prefix_);

    /// Return the array of exported property prefixes
    const std::vector<std::string> & get_auxiliary_property_prefixes() const;

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool debug = true);

    /// Check the initialization flag
    bool is_initialized() const;

    /// Initialize the manager
    void initialize();

    /// Initialize the manager from a container of properties
    void initialize(const datatools::properties& config);

    /// Reset the manager
    void reset();

    bool has_component_type(const std::string& id) const;

    template <class ComponentClass>
    void register_component_type(const std::string& id);

    void unregister_component_type(const std::string& id);

    /**  @param name The name of the component to be checked
     *   @return true if the manager hosts the component requested by name
     */
    bool has(const std::string& name) const;

    /**  @param name The name of the component to be checked
     *   @return true if the component is initialized
     */
    bool is_initialized(const std::string& name) const;

    /**  @param name The name of the component to be checked
     *   @return true if the component is of the requested type
     */
    template <class T>
    bool is_a(const std::string& name) const;

    /**  Same as the mutable 'get' method
     *   @param name The name of the component to be checked
     *   @return a mutable reference to the component instance requested by name and type
     */
    template<class T>
    T& grab(const std::string& name);

    /**  @param name The name of the component to be checked
     *   @return a const reference to the component instance requested by name and type
     */
    template<class T>
    const T& get(const std::string& name) const;

    void load(const std::string& name,
              const std::string& id,
              const datatools::properties& config);

    void load(const datatools::multi_properties& config);

    const geomtools::id_mgr & get_eid_manager() const;

    void load_eid_categories(const std::string &);

    const component_pool_type& get_components() const;

    component_pool_type& grab_components();

    void dump_components(std::ostream& out = std::clog,
                         const std::string& title  = "",
                         const std::string& indent = "") const;

    /// Smart print
    virtual void tree_dump(std::ostream& out         = std::clog,
                           const std::string& title  = "",
                           const std::string& indent = "",
                           bool inherit              = false) const;

    void set_logging_priority(datatools::logger::priority);

    datatools::logger::priority get_logging_priority() const;

    void load_component(const std::string& name,
                        const std::string& id,
                        const datatools::properties& config);

    void create_component(component_entry& entry);

    void initialize_component(component_entry& entry);

    void reset_component(component_entry& entry);

  protected:

    void _do_preload_global_dict();

  private:

    datatools::logger::priority _logging_priority_; //!< Logging priority threshold
    bool         _initialized_;       //!< Initialization flag
    std::string  _setup_label_;       //!< the label associated to the setup
    std::string  _setup_version_;     //!< the version tag of the setup
    std::string  _setup_description_; //!< the description of the setup
    bool         _preload_;           //!< Factory preload flag
    bool         _preloaded_;         //!< Factory preload
    bool         _force_initialization_at_load_; //!< Flag for triggering component  initialization at load (rather than first use)
    std::vector<std::string> _auxiliary_property_prefixes_; //!< List of property prefixes to be preserved in component models auxiliary properties

    component_model_base::factory_register_type  _factory_register_; //!< Factory register
    component_pool_type _components_;       //!< Dictionary of components
    geomtools::id_mgr _eid_manager_; //!< manager of electronics ID
    bool              _mapping_requested_; //!< Mapping build request flag
    mapping           _mapping_; //!< Default mapping
    // Mapping...

  };

} // end of namespace electronics

#include <electronics/component_manager-inl.h>

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(electronics::component_manager)

#endif // ELECTRONICS_COMPONENT_MANAGER_H_
