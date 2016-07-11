/// \file datatools/service_manager.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2015-12-03
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * History:
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
  //!
  //!
  class service_manager : public datatools::i_tree_dumpable {
  public:
    enum flag_type {
      BLANK              = 0,
      NO_PRELOAD         = datatools::bit_mask::bit00,
      FORCE_INITIALIZATION_AT_LOAD = datatools::bit_mask::bit01,
      FACTORY_VERBOSE    = datatools::bit_mask::bit02
    };

  public:
    //! Constructor
    service_manager(const std::string& name = "",
                    const std::string & description = "",
                    uint32_t flag = BLANK);

    //! Destructor
    virtual ~service_manager();

    //! Set the name of the service
    void set_name(const std::string& name);

    //! Set the description of the service
    void set_description(const std::string& description);

    //! Get the name of the service
    const std::string& get_name() const;

    //! Get the description of the service
    const std::string& get_description() const;

    //! Check the debug flag
    bool is_debug() const;

    //! Set the debug flag
    void set_debug(bool debug = true);

    //! Check the initialization flag
    bool is_initialized() const;

    //! Initialize the manager
    void initialize();

    //! Initialize the manager from a container of properties
    void initialize(const datatools::properties& config);

    //! Reset the manager
    void reset();

    //! Check if a service identifier/type is supported
    bool has_service_type(const std::string& id) const;

    //! Register a service factory
    template <class ServiceClass>
    void register_service_type(const std::string& id);

    //! Unregister a service factory
    void unregister_service_type(const std::string& id);

    /**  @param name The name of the service to be checked
     *   @return true if the manager hosts the service requested by name
     */
    bool has(const std::string& name) const;

    /**  @param name The name of the service to be checked
     *   @return true if the service is initialized
     */
    bool is_initialized(const std::string& name) const;

    /**  @param name The name of the service to be checked
     *   @return true if the service is of the requested type
     */
    template <class T>
    bool is_a(const std::string& name) const;

    /**  Same as the mutable 'get' method
     *   @param name The name of the service to be checked
     *   @return a mutable reference to the service instance requested by name and type
     */
    template<class T>
    T& grab(const std::string& name);

    /**  @param name The name of the service to be checked
     *   @return a const reference to the service instance requested by name and type
     */
    template<class T>
    const T& get(const std::string& name) const;

    /**  @param name The name of the service to be checked
     *   @return a mutable reference to the service instance requested by name
     */
    base_service & grab_service(const std::string& name);

    /**  @param name The name of the service to be checked
     *   @return a const reference to the service instance requested by name
     */
    const base_service & get_service(const std::string& name) const;

    //! Check if a service with given name can be dropped
    bool can_drop(const std::string& name);

    //! Drop a service given its name
    void drop(const std::string& name);

    //! Load a service with given name, identifier and configuration
    void load(const std::string& name,
              const std::string& id,
              const datatools::properties& config);

    //! Load a set of services from a multi-service configuration
    void load(const datatools::multi_properties& config);

    //! Return a reference to the non mutable dictionary of services
    const service_dict_type& get_services() const;

    //! Return a reference to the mutable dictionary of services
    service_dict_type& grab_services();

    //! Basic print of embedded services
    void dump_services(std::ostream& out = std::clog,
                       const std::string& title  = "",
                       const std::string& indent = "") const;

    //! Smart print
    virtual void tree_dump(std::ostream& out         = std::clog,
         const std::string& title  = "",
         const std::string& indent = "",
         bool inherit              = false) const;

    //! Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

    //! Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

  protected:

    //! Load a service
    void load_service(const std::string& name,
                      const std::string& id,
                      const datatools::properties& config);

    //! Preload the factory register from service factory system register
    void preload_global_dict();

    //! Instantiate a service from its entry
    void create_service(service_entry& entry);

    //! Initialize a service from its entry
    void initialize_service(service_entry& entry);

    //! Reset a service from its entry
    void reset_service(service_entry& entry);

  private:

    //! Set the factory preload flag
    void set_preload(bool preload);

  private:

    datatools::logger::priority _logging_priority; //!< Logging priority threshold
    bool         initialized_; //!< Initialization flag
    std::string  name_; //!< Manager's name
    std::string  description_; //!< Manager's description
    bool         preload_; //!< Factory preload flag
    bool         force_initialization_at_load_; //!< Flag for triggering service  initialization at load (rather than first use)

    // 2012-04-09 FM : support for datatools::factory system :
    base_service::factory_register_type  factory_register_;
    service_dict_type                    services_; //!< Dictionary of services
  };

}  // end of namespace datatools

#include <datatools/service_manager-inl.h>

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(datatools::service_manager)

#endif // DATATOOLS_SERVICE_MANAGER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
