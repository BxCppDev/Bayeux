/// \file datatools/service_tools.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2016-12-18
 *
 * Copyright (C) 2011-2018 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Service tools, typedefs.
 *
 * History:
 *
 */
#ifndef DATATOOLS_SERVICE_TOOLS_H
#define DATATOOLS_SERVICE_TOOLS_H

// Standard Library:
#include <string>
#include <map>
#include <vector>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/bit_mask.h>

namespace datatools {

  /// \brief Base class for all services
  class base_service;
  typedef datatools::handle<base_service> service_handle_type;

  //! \brief Constants to measure the level of dependance between services
  enum dependency_level_type {
    DEPENDENCY_UNKNOWN  = -1, //! Unknown/undefined dependency relationship
    DEPENDENCY_NONE     =  0, //!< The service does not depend on the external service
    DEPENDENCY_OPTIONAL =  1, //!< The service can work without the external service
    DEPENDENCY_WEAK     =  2, //!< Not so strong dependency on the external service (however part of the service may be invalidated)
    DEPENDENCY_STRICT   =  3  //!< Strictly depends on the external service
  };

  //! \brief Record that stores informations about the dependency between services :
  struct dependency_info_type {
    std::string id;              //!< ID of the external service
    std::string version;         //!< Version of the external service
    std::string meta;            //!< Auxiliary information
    dependency_level_type level; //!< Level of the dependency (see dependency_level_type enum)
    dependency_info_type();
    void reset();
  };

  // Long typedef names
  typedef std::map<std::string, int> dependency_level_dict_type;
  typedef std::map<std::string, dependency_info_type> service_dependency_dict_type;

  class service_manager;

  //! \brief Internal entry for service objects stored in the service manager class
  //!
  //! Record that handles a dynamically allocated and initialized service and additional
  //! informations
  class service_entry : public datatools::i_tree_dumpable  {
  public:

    //! \brief Service status flags
    enum status_type {
      STATUS_BLANK             = 0, //!< Empty flag set
      STATUS_CREATED           = datatools::bit_mask::bit00, //!< Creation/instantiation flag
      STATUS_INITIALIZED       = datatools::bit_mask::bit01, //!< Initialization flag
      STATUS_BROKEN_DEPENDENCY = datatools::bit_mask::bit02  //!< Broken dependency flag
    };

  public:

    //! Return the service name
    const std::string & get_service_name() const;

    //! Set the service name
    void set_service_name(const std::string &);

    //! Return the service identifier
    const std::string & get_service_id() const;

    //! Set the service identifier
    void set_service_id(const std::string &);

    //! Return a reference to the non mutable service configuration
    const datatools::properties & get_service_config() const;

    //! Return a reference to the mutable service configuration
    datatools::properties & grab_service_config();

    //! Set the service configuration
    void set_service_config(const datatools::properties &);

    //! Default constructor
    service_entry();

    //! Check if service can be dropped
    bool can_be_dropped() const;

    //! Return the service status
    uint32_t get_service_status() const;

    //! Update the service status
    void update_service_status(uint32_t);

    //! Reset the service status
    void reset_service_status(uint32_t);

    //! Check if the service object is instantiated
    bool is_created() const;

    //! Check if the service object is initialized
    bool is_initialized() const;

    //! Check if the service object has slave service with given name
    bool has_slave(const std::string& name) const;

    //! Remove slave service with given name
    void remove_slave(const std::string& name);

    //! Smart print
    virtual void tree_dump(std::ostream& out = std::clog,
                           const std::string & title  = "",
                           const std::string & indent = "",
                           bool inherit = false) const;

    //! Return a handle to the non mutable service
    const service_handle_type & get_service_handle() const;

    //! Return a handle to the mutable service
    service_handle_type & grab_service_handle();

    //! Check if service manager is available
    bool has_service_manager() const;

    //! Set the service manager
    void set_service_manager(service_manager & smgr);

    //! Return a reference to the service manager
    const service_manager & get_service_manager() const;

    //! Return a mutable reference to the service manager
    service_manager & grab_service_manager();

  private:
    std::string           service_name;   //!< The name of the service
    std::string           service_id;     //!< The ID (type) of the service
    datatools::properties service_config; //!< The configuration of the service
    uint32_t              service_status; //!< The status of the service
    service_handle_type   service_handle; //!< The handle for the allocated service
    service_manager *     manager;        //!< The handle to the host service manager
  public:
    service_dependency_dict_type service_masters; //!< The list of services the service depends on (by names)
    dependency_level_dict_type   service_slaves;  //!< The list of depending services (by names)
  };

  //! \brief Type alias for a dictionary of service entries
  typedef std::map<std::string, service_entry> service_dict_type;

}  // end of namespace datatools

#endif // DATATOOLS_SERVICE_TOOLS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
