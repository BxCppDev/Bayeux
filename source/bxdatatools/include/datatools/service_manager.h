/// \file datatools/service_manager.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2017-11-02
 *
 * Copyright (C) 2011-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Copyright (C) 2012 Ben Morgan <Ben.Morgan@warwick.ac.uk>
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
 *
 * Description:
 *
 *   A service manager.
 *
 */

#ifndef DATATOOLS_SERVICE_MANAGER_H
#define DATATOOLS_SERVICE_MANAGER_H

// Standard Library:
#include <string>
#include <sstream>
#include <typeinfo>
#include <stdexcept>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <datatools/service_tools.h>
#include <datatools/base_service.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/bit_mask.h>

namespace datatools {

  class properties;
  class multi_properties;

  //! \brief Service management class
  class service_manager
    : public datatools::i_tree_dumpable
  {
  public:

    enum flag_type {
      BLANK                        = 0,                          ///< No special configuration flags
      NO_PRELOAD                   = datatools::bit_mask::bit00, ///< Do not preload service types from the system register
      FORCE_INITIALIZATION_AT_LOAD = datatools::bit_mask::bit01, ///< Force initialization of service instance at load rather than at first use
      FACTORY_VERBOSE              = datatools::bit_mask::bit02, ///< Activate factory verbosity (debug)
      ALLOW_DYNAMIC_SERVICES       = datatools::bit_mask::bit03  ///< Allow dynamic services
    };

    /*
    struct dependee_manager_handle
    {
      dependee_manager_handle();
      ~dependee_manager_handle();
      void initialize(const service_manager &);
      void initialize(const datatools::properties & config_,
                      const std::string & name_ = "",
                      const std::string & description_ = "",
                      const uint32_t flags_ = 0);
      void reset();
      bool is_initialized() const;
      bool is_owned() const;
      const service_manager & get_manager() const;
    private:
      const service_manager * _manager_ = nullptr; //!< Handle to the dependee manager
      bool                    _owned_   = false;   //!< Dependee manager is managed by the handle itself
    };
    */

    // typedef std::map<std::string, dependee_manager_handle> mgr_dict_type;

    //! Constructor
    service_manager(const std::string & name_ = "",
                    const std::string & description_ = "",
                    uint32_t flag_ = BLANK);

    //! Destructor
    ~service_manager() override;

    //! Set the name of the service
    void set_name(const std::string& name_);

    //! Set the description of the service
    void set_description(const std::string& description_);

    //! Get the name of the service
    const std::string& get_name() const;

    //! Get the description of the service
    const std::string& get_description() const;

    //! Check the debug flag
    bool is_debug() const;

    //! Set the debug flag
    void set_debug(bool debug_ = true);

    //! Set the flag to allow_dynamic_services
    void set_allow_dynamic_services(bool);

    //! Check if dynamic services are allowed
    bool is_allow_dynamic_services() const;

    //! Set the flag to force initialization of services at load
    void set_force_initialization_at_load(bool);

    //! Check if the flag to force initialization of services at load is set
    bool is_force_initialization_at_load() const;

    //! Check the initialization flag
    bool is_initialized() const;

    //! Initialize the manager
    void initialize();

    //! Initialize the manager from a container of properties
    void initialize(const datatools::properties& config_);

    //! Reset the manager
    void reset();

    // //! Check if external dependee managers are mounted
    // bool has_mounted_dependee_managers() const;

    // //! Check if external dependee manager can be mounted
    // bool can_mount_dependee_manager() const;

    // //! Mount an external dependee manager
    // void mount_dependee_manager(const service_manager &);

    // //! Unmount an external dependee managers
    // void unmount_dependee_managers();

    //! Check if a service identifier/type is supported
    bool has_service_type(const std::string& id_) const;

    //! Register a service factory
    template <class ServiceClass>
    void register_service_type(const std::string& id_);

    //! Unregister a service factory
    void unregister_service_type(const std::string& id_);

    //! \brief Special flag to build the list of service names
    enum service_filter_flag {
       FILTER_NONE             = 0,                          ///< No special filter flags
       FILTER_NO_INITIALIZED   = datatools::bit_mask::bit00, ///< Flag to skip initialized services
       FILTER_NO_UNINITIALIZED = datatools::bit_mask::bit01, ///< Flag to skip uninitialized services
       FILTER_NO_CLEAR         = datatools::bit_mask::bit02, ///< Flag to skip clear the list
       FILTER_FROM_BUS         = datatools::bit_mask::bit03  ///< Flag to take into account services from the bus
    };

    //! Build the list of services names
    void build_list_of_services(std::vector<std::string> & list_, uint32_t flags_ = 0);

    /**  @param name_ The name of the service to be checked
     *   @return true if the manager hosts the service requested by name
     */
    bool has(const std::string & name_) const;

    /**  @param name_ The name of the service to be checked
     *   @return true if the service is initialized
     */
    bool is_initialized(const std::string & name_) const;

    /**  @param name_ The name of the service to be checked
     *   @return true if the service is of the requested type
     */
    template <class T>
    bool is_a(const std::string & name_) const;

    /**  Same as the mutable 'get' method
     *   @param name_ The name of the service to be checked
     *   @return a mutable reference to the service instance requested by name and type
     */
    template<class T>
    T& grab(const std::string & name_);

    /**  @param name_ The name of the service to be checked
     *   @return a const reference to the service instance requested by name and type
     */
    template<class T>
    const T& get(const std::string & name_) const;

    /**  @param name_ The name of the service to be checked
     *   @return a mutable reference to the service instance requested by name
     */
    base_service & grab_service(const std::string & name_);

    /**  @param name_ The name of the service to be checked
     *   @return a const reference to the service instance requested by name
     */
    const base_service & get_service(const std::string & name_) const;

    //! Check if a service with given name can be dropped
    bool can_drop(const std::string & name_) const;

    //! Drop a service given its name
    void drop(const std::string & name_);

    //! Load a service with given name, identifier and configuration
    void load(const std::string & name_,
              const std::string & id_,
              const datatools::properties & config_);

    //! Load a service with given name and identifier and without initialization
    //!
    //! Initialization must be manually done later through the returned reference
    //! This method is reserved for experts only because it uses the
    //! interface of the service manager in a very special way.
    base_service & load_no_init(const std::string & name_,
                                const std::string & id_);

    //! Configure an uninitialized service
    //!
    //! This method is reserved for experts only because it uses the
    //! interface of the service manager in a very special way.
    void configure_no_init(const std::string& name_, const datatools::properties& config_);

    //! Load a set of services from a multi-service configuration
    void load(const datatools::multi_properties& config_);

    //! Return the list of services locally hosted by this manager
    const service_dict_type& get_local_services() const;

    //! Return the bus of services known in the context of this manager
    const service_dict_type& get_bus_of_services(const bool update_ = false) const;

    //! Basic print of embedded services
    void dump_services(std::ostream & out_ = std::clog,
                       const std::string& title_  = "",
                       const std::string& indent_ = "") const;

    //! Smart print
    void tree_dump(std::ostream& out_         = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_              = false) const override;

    //! Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

    //! Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    //! Update the bus of services (expert only)
    void update_service_bus();

    //! Synchronize internal data (expert only)
    void sync();

  protected:

    //! Load a service
    void load_service(const std::string& name_,
                      const std::string& id_,
                      const datatools::properties& config_);

    //! Load a service with or without initialization
    void load_service(const std::string& name_,
                      const std::string& id_,
                      const datatools::properties * config_ptr_);

    //! Preload the factory register from service factory system register
    void preload_global_dict();

    //! Instantiate a service from its entry
    void create_service(service_entry& entry_);

    //! Initialize a service from its entry
    void initialize_service(service_entry& entry_);

    //! Reset a service from its entry
    void reset_service(service_entry& entry_);

    //! Destroy a service from its entry
    void destroy_service(service_entry& entry_);

    //! Action done at service creation
    void at_service_creation(const std::string & service_name_, const std::string & service_type_id_);

    //! Action done at service initialization
    void at_service_initialization(const std::string & service_name_);

    //! Action done at service reset
    void at_service_reset(const std::string & service_name_);

    //! Action done at service drop
    void at_service_drop(const std::string & service_name_);

  private:

    //! Set the factory preload flag
    void set_preload(bool preload_);

  private:

    datatools::logger::priority _logging_priority_ = datatools::logger::PRIO_FATAL; //!< Logging priority threshold
    bool         _initialized_ = false; //!< Initialization flag
    std::string  _name_;                //!< Manager's name
    std::string  _description_;         //!< Manager's description
    bool         _preload_ = false;     //!< Factory preload flag
    bool         _force_initialization_at_load_ = false; //!< Flag for triggering service initialization at load (rather than first use)
    bool         _allow_dynamic_services_ = false;       //!< Flag to allow dynamic services

    // 2012-04-09 FM : support for datatools::factory system :
    base_service::factory_register_type  _factory_register_;
    // mgr_dict_type                        _dependee_managers_; //!< Dictionary of dependee managers

    service_dict_type                    _local_services_; //!< Dictionary of services managed locally

    service_dict_type                    _service_bus_; //!< Bus of all services known from this manager

    friend class service_entry;
  };

}  // end of namespace datatools

#include <datatools/service_manager-inl.h>

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(datatools::service_manager)

#endif // DATATOOLS_SERVICE_MANAGER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
