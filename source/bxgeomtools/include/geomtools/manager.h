/// \file geomtools/manager.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-21
 * Last modified: 2013-02-19
 *
 * License:
 *
 * Copyright 2007-2013 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Geometry manager
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MANAGER_H
#define GEOMTOOLS_MANAGER_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/service_tools.h>
#include <datatools/factory_macros.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/shape_factory.h>
#include <geomtools/model_factory.h>
#include <geomtools/id_mgr.h>
#include <geomtools/mapping.h>

namespace datatools {
  // Forward declaration:
  class version_id;
}

namespace geomtools {

  /// \brief Geometry manager for virtual geometry modelling.
  /// Main geometry manager for the modelisation of various
  /// experimental setups in the framework of the nuclear and particle
  /// physics experiments.
  class manager : public datatools::i_tree_dumpable
  {
  public:

    // Forward declaration:
    class plugin_entry;

    /// Dictionary of plugin entries
    typedef std::map<std::string, plugin_entry> plugins_dict_type;

    /// \brief The geometry manager's plugin base class
    class base_plugin
    {
    public:

      /// Constructor
      base_plugin();

      /// Destructor
      virtual ~base_plugin();

      /// Set the reference to the geometry manager
      void set_geo_manager(manager & geo_mgr_);

      /// Get a non-mutable reference to the geometry manager
      const manager & get_geo_manager() const;

      /// Initialize the plugin using only a list of properties without the needs of other resources
      virtual int initialize_standalone(const datatools::properties& config);

      /// Initialize the plugin using a list of properties and a list of other plugins
      virtual int initialize_simple(const datatools::properties & config_,
                                    const plugins_dict_type & plugins_);

      /// Main initialization method from a list of properties, a list of other plugins and a list of services
      virtual int initialize(const datatools::properties & config_,
                             const plugins_dict_type & plugins_,
                             const datatools::service_dict_type & services_) = 0;

      /// Main reset method
      virtual int reset() = 0;

      /// Check if plugin is initialized
      virtual bool is_initialized() const = 0;

      /// Get the logging priority threshold
      datatools::logger::priority get_logging_priority() const;

      /// Set the logging priority threshold
      void set_logging_priority( datatools::logger::priority);

    protected:

      /// Basic initialization
      void _basic_initialize(const datatools::properties & config_);

      /// Get a mutable reference to the geometry manager
      manager & _grab_geo_manager();

      /// Check presence of geometry requirements
      bool _has_geom_setup_requirement() const;

      /// Check geometry requirement
      void _check_geom_setup_requirement() const;

      datatools::logger::priority _logging; /// The logging priority threshold

    private:

      manager * _geo_mgr_;                                //!< Handle to the geometry manager
      std::vector<std::string> _geom_setup_requirements_; //!< Minimal version of geometry setups

      // Factory stuff :
      DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_plugin)
    };

    typedef datatools::handle<base_plugin> plugin_handle_type;

    /// \brief Internal record plugin handling
    class plugin_entry : public datatools::i_tree_dumpable
    {
    public:

      /// \brief Status of a plugin entry
      enum status_type {
        STATUS_BLANK        = 0x0, //!< Not created not initialized
        STATUS_CREATED      = 0x1, //!< Created
        STATUS_INITIALIZED  = 0x2 //!< Initialized
      };

      /// Set the name of the plugin
      void set_name(const std::string&);

      /// Get the name of the plugin
      const std::string& get_name() const;

      /// Get the ID of the plugin
      const std::string& get_id() const;

      /// Set the description of the plugin
      void set_description(const std::string&);

      /// Check is the description is not empty
      bool has_description() const;

      /// Get the description of the plugin
      const std::string& get_description() const;

      /// Set the configuration
      const datatools::properties & get_config() const;

      /// Get the configuration
      void set_config(const datatools::properties &);

      /// Grab the configuration
      datatools::properties & grab_config();

      /// Check if plugin is created
      bool is_created() const;

      /// Check if plugin is initialized
      bool is_initialized() const;

      /// Check if plugin can be dropped by the manager
      bool can_be_dropped() const;

      /// Default constructor
      plugin_entry();

      /// Destructor
      ~plugin_entry() override;

      /// Return a reference to the mutable plugin
      base_plugin & grab();

      /// Return a reference to the non mutable plugin
      const base_plugin & get() const;

      /// Smart print
      void tree_dump(std::ostream &      out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool                inherit_ = false) const override;

    protected:

      /// Modify the status of the plugin entry
      void _set_status(uint32_t);

    public:

      std::string        _name_;        //!< Name of the plugin
      std::string        _id_;          //!< The ID (type) of the plugin
      std::string        _description_; //!< Description of the plugin
      datatools::properties _config_;   //!< The configuration of the plugin
      uint32_t           _status_;      //!< Status of the plugin
      plugin_handle_type _handle_;      //!< Handle to the plugin

      // friend class manager;
    };

    /// Check the debug flag
    bool is_debug() const;

    /// Check the verbose flag
    bool is_verbose() const;

    /// Check the initialization flag
    bool is_initialized() const;

    /// Check if manager has services
    bool has_services() const;

    /// Set the dictionary of services
    void set_services(datatools::service_dict_type & services_);

    /// Reset the dictionary of services
    void reset_services();

    /// Set the label of the setup
    void set_setup_label(const std::string &);

    /// Return the label of the setup
    const std::string & get_setup_label() const;

    /// Set the version of the setup
    void set_setup_version(const std::string &);

    /// Return the version of the setup
    const std::string & get_setup_version() const;

    /// Set the description of the setup
    void set_setup_description(const std::string &);

    /// Return the description of the setup
    const std::string & get_setup_description() const;

    /// Construct the version id associated to the setup
    void fetch_setup_version_id(datatools::version_id & vid_) const;

    /// Request the build of the mapping
    void set_mapping_requested(bool);

    /// Check if the build of the mapping has been requested
    bool is_mapping_requested() const;

    /// Check if the mapping is built
    bool is_mapping_available() const;

    /// Set the name of the world volume
    void set_world_name(const std::string &);

    /// Return the name of the world volume
    const std::string & get_world_name() const;

    /// Build the mapping
    void build_mapping(const datatools::properties & config_);

    /// Return a reference to the non mutable factory of shapes
    const geomtools::shape_factory & get_shape_factory() const;

    /// Return a reference to the mutable factory of shapes
    geomtools::shape_factory & grab_shape_factory();

    /// Return a reference to the non mutable factory of geometry models
    const geomtools::model_factory & get_factory() const;

    /// Return a reference to the mutable factory of geometry models
    geomtools::model_factory & grab_factory();

    /// Return a reference to the non mutable geometry ID manager
    const geomtools::id_mgr & get_id_mgr() const;

    /// Return a reference to the mutable geometry ID manager
    geomtools::id_mgr & grab_id_mgr();

    /// Return a reference to the non mutable mapping with explicit name
    const geomtools::mapping & get_mapping(const std::string & mapping_name_ = "") const;

    /* Plugins management */

    bool can_drop_plugin(const std::string& plugin_name_);

    void drop_plugin(const std::string& plugin_name_);

    void load_plugin(const std::string& plugin_name_,
                     const std::string& plugin_id_,
                     const datatools::properties& plugin_config_,
                     bool only_lock_ = false);

    void load_plugins(const datatools::multi_properties& plugin_config_,
                      bool only_lock_ = false);

    const plugins_dict_type & get_plugins() const;

    plugins_dict_type & grab_plugins();

    /**  @param a_plugin_name The name of the plugin to be checked
     *   @return true if the plugin is of the requested type
     */
    template<class T>
    bool is_plugin_a(const std::string & a_plugin_name) const;

    /** @param a_plugin_name The name of the plugin to be grabbed
     *  @return a mutable reference to the plugin instance requested by name and type
     */
    template<class T>
    T& grab_plugin(const std::string & a_plugin_name);

    /**  @param a_plugin_name The name of the plugin to be grabbed
     *   @return a non-mutable reference to the plugin instance requested by name and type
     */
    template<class T>
    const T& get_plugin(const std::string & a_plugin_name) const;

    /**  @param a_plugin_name The name of the plugin to be checked
     *   @return true if the manager hosts the plugin requested by name
     */
    bool has_plugin(const std::string & a_plugin_name) const;

    void print_plugins(std::ostream& out = std::clog,
                       const std::string& title  = "",
                       const std::string& indent = "") const;

    /// Clear the plugins
    void clear_plugins();

    /// Check if some plugin type is known
    bool has_plugin_type(const std::string& plugin_id) const;

    /// Register a plugin type
    template <class PluginClass>
    void register_plugin_type(const std::string& plugin_id);

    /// Unregister a plugin type
    void unregister_plugin_type(const std::string& plugin_id);

    /* Main interface */

    /// Default constructor
    manager();

    /// Destructor
    ~manager() override;

    /// Initialize the geometry manager from a container of properties
    void init(const datatools::properties & config_);

    /// Initialize the geometry manager from a container of properties
    void initialize(const datatools::properties & config_);

    /// Reset the geometry manager
    void reset();

    /// Smart print
    void tree_dump(std::ostream &      out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool                inherit_ = false) const override;

    //! Return a reference to the plugin factory register
    const base_plugin::factory_register_type & get_plugins_factory_register();

    //! Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority a_logging_priority);

    //! Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    //! Print the list of geometry identifiers (GIDs) from a manager and given specific rules
    static int print_list_of_gids(const geomtools::manager & mgr_,
                                  const std::string & command_,
                                  const std::string & rules_ = "",
                                  std::ostream & out_ = std::clog);

  protected:

    virtual void _pre_init(const datatools::properties & config_);

    virtual void _post_init(const datatools::properties & config_);

    void _load_plugin(const std::string& name,
                      const std::string& id,
                      const datatools::properties& config);

    void _preload_plugins_global_dict();

    void _create_plugin(plugin_entry& entry);

    void _initialize_plugin(plugin_entry& entry);

    void _reset_plugin(plugin_entry& entry);

  private:

    /// Core initialization private method (wrapped by 'initialize')
    virtual void _at_init_(const datatools::properties & config_);

    /// Plugins initialization private method
    void _init_plugins_(const datatools::properties & config_);

  protected:

    datatools::logger::priority _logging; //!< Logging priority threshold

  private:

    bool                     _initialized_;       //!< initialization flag
    std::string              _setup_label_;       //!< the label associated to the geometry setup
    std::string              _setup_version_;     //!< the version tag of the geometry setup
    std::string              _setup_description_; //!< the description of the geometry setup

    datatools::service_dict_type * _services_;    //!< Handle to a dictionary of services

    geomtools::shape_factory _shape_factory_;     //!< the factory for 3D shapes
    geomtools::model_factory _factory_;           //!< the factory for geometry models
    geomtools::id_mgr        _id_manager_;        //!< the manager for geometry IDs
    bool                     _mapping_requested_; //!< flag for building mapping
    geomtools::mapping       _mapping_;           //!< the mapping manager
    datatools::multi_properties _external_mapping_rules_; //!< Mapping rules associated to geometry models

    std::string              _world_name_;        //!< the name of the 'world' model

    bool                                _plugins_factory_preload_;  //!< Flag for preloading of plugins system factory
    bool                                _plugins_force_initialization_at_load_; //!< Flag to enforce initialization of plugins at load
    base_plugin::factory_register_type  _plugins_factory_register_; //!< Plugins registration
    plugins_dict_type                   _plugins_;                  //!< Plugins dictionary

  };

} // end of namespace geomtools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::manager)

#include <geomtools/detail/manager-inl.h>

#endif // GEOMTOOLS_MANAGER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
