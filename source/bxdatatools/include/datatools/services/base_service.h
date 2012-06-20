/* base_service.h
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
 * Description:
 *
 *   Base service.
 *
 * History:
 *
 */
#ifndef DATATOOLS_SERVICE_BASE_SERVICE_H_
#define DATATOOLS_SERVICE_BASE_SERVICE_H_

// Standard Library
#include <string>
#include <iostream>

// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>

// Datatools
#include <datatools/utils/i_tree_dump.h>
#include <datatools/services/service_tools.h>
#include <datatools/factory/factory_macros.h>

namespace datatools {
namespace utils {
class properties;
}

namespace service {
class base_service : public datatools::utils::i_tree_dumpable {
 public:
  static bool g_debug;

 public:
  /// Constructor
  base_service(const std::string& name, const std::string& description = "",
               const std::string& version = "");

  /// Destructor
  virtual ~base_service();

  const std::string& get_name() const;

  bool has_description() const;

  const std::string& get_description() const;

  void set_description(const std::string& description);

  bool has_version() const;

  const std::string& get_version() const;

  void set_version(const std::string& version);

  virtual void fetch_dependencies(
      service_dependency_dict_type& /*dependencies*/) const;

  virtual bool is_initialized() const = 0;

  virtual int initialize_standalone(
      const datatools::utils::properties& config);

  virtual int initialize(const datatools::utils::properties& config,
                         service_dict_type& service_dict) = 0;

  virtual int reset() = 0;

 protected:
  void set_name(const std::string& name);

  virtual void tree_dump(std::ostream& out = std::clog,
                         const std::string & title = "",
                         const std::string & indent = "",
                         bool inherit = false) const;

 protected:
  std::string name_;         //!< The name of the service
  std::string description_;  //!< The description of the service
  std::string version_;      //!< The version of the service

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_service);
};
      
}  // end of namespace service
}  // end of namespace datatools

#endif // DATATOOLS_SERVICE_BASE_SERVICE_H_

