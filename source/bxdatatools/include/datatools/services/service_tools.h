/* service_tools.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2011-06-07
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * 
 * Description: 
 *
 *   Service tools, typedefs.
 * 
 * History: 
 * 
 */
#ifndef DATATOOLS_SERVICES_SERVICE_TOOLS_H_
#define DATATOOLS_SERVICES_SERVICE_TOOLS_H_

// Standard Library
#include <string>
#include <map>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// Datatools
#include <datatools/utils/handle.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>

namespace datatools {
namespace service {

class base_service;
typedef datatools::utils::handle<base_service> service_handle_type;

// Constants to measure the level of dependance between services
enum dependency_level_type {
  NO_DEPENDENCY       =  0, //!< The service does not depend on the external service
  OPTIONAL_DEPENDENCY =  1, //!< The service can work without the external service
  WEAK_DEPENDENCY     =  2, //!< Not so strong dependency on the external service (however part of the service may be invalidated)
  STRICT_DEPENDENCY   =  3, //!< Strictly depends on the external service
  UNKNWON_DEPENDENCY  = -1
};

// Record that store informations about the dependency between services :
struct dependency_info_type {
  std::string id;      //!< ID of the external service
  std::string version; //!< Version of the external service
  std::string meta;    //!< Auxiliary information 
  int level;           //!< Level of the dependency (see dependency_level_type enum)
  dependency_info_type();
};

// HA! The typedef names are JUST AS LONG!!!!
typedef std::map<std::string, int> dependency_level_dict_type;
typedef std::map<std::string, dependency_info_type> service_dependency_dict_type;


// Record that handle an dynamically allocated service and additional 
// informations :
class service_entry : public datatools::utils::i_tree_dumpable  {
 public:
  enum status_type {
    STATUS_BLANK             = 0x0,
    STATUS_CREATED           = 0x1,
    STATUS_INITIALIZED       = 0x2,
    STATUS_BROKEN_DEPENDENCY = 0x4
  };

  // WHY ARE THESE PUBLIC?? service_entry has functionality, it's
  // not just a bag of data...
  // Either that, or it should be a pImpl of service_manager
  //
  std::string  service_name;    //!< The name of the service
  std::string  service_id;      //!< The ID (type) of the service
  datatools::utils::properties service_config;  //!< The configuration of the service 
  uint32_t service_status;  //!< The status of the service
  service_handle_type service_handle;  //!< The handle for the allocated service
  service_dependency_dict_type service_masters; //!< The list of services the service depends on (by names)
  dependency_level_dict_type   service_slaves;  //!< The list of depending services (by names)

 public:
  service_entry();

  bool can_be_dropped() const;

  bool has_slave(const std::string& name) const;

  void remove_slave(const std::string& name);

  virtual void tree_dump(std::ostream& out = std::clog, 
                         const std::string & title  = "",
                         const std::string & indent = "",
                         bool inherit = false) const;

};

typedef std::map<std::string, service_entry> service_dict_type;

}  // end of namespace service
}  // end of namespace datatools

#endif // DATATOOLS_SERVICES_SERVICE_TOOLS_H_

