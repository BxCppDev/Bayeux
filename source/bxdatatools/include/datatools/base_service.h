/// \file datatools/base_service.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2015-04-09
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_BASE_SERVICE_H
#define DATATOOLS_BASE_SERVICE_H

// Standard Library
#include <string>
#include <iostream>

// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>

// This project (Bayeux/datatools)
#include <datatools/i_tree_dump.h>
#include <datatools/service_tools.h>
#include <datatools/factory_macros.h>
#include <datatools/logger.h>

namespace datatools {

  // Forward class declaration:
class properties;

/*! \brief The base service class
 *
 *  The base_service class provides the interface of all
 *  datatools-based service objects.
 */
class base_service : public datatools::i_tree_dumpable {
 public:

  /// Default constructor
  base_service();

  /// Constructor
  base_service(const std::string& name, const std::string& description = "",
               const std::string& version = "");

  /// Destructor
  virtual ~base_service();

  /// Set the name of the service
  void set_name(const std::string&);

  /// Get the name of the service
  const std::string& get_name() const;

  /// Check is the description is not empty
  bool has_description() const;

  /// Get the description of the service
  const std::string& get_description() const;

  /// Set the description of the service
  void set_description(const std::string& description);

  /// Check is the version ID is not empty
  bool has_version() const;

  /// Get the version ID of the service
  const std::string& get_version() const;

  /// Set the version ID of the service
  void set_version(const std::string& version);

  /// Undocumented unused/unimplemented method
  virtual void fetch_dependencies(service_dependency_dict_type& /*dependencies*/) const;

  /// Initialize the service using only a list of properties without the needs of other services
  virtual int initialize_standalone(const datatools::properties& config);

  /// Initialize the service using a list of properties with access to a dictionry of other services
  virtual int initialize(const datatools::properties& config,
                         service_dict_type& service_dict) = 0;

  /// Reset the service
  virtual int reset() = 0;

  /// Check if service is initialized
  virtual bool is_initialized() const = 0;

  /// Set the logging priority threshold
  void set_logging_priority(datatools::logger::priority);

  /// Return the logging priority threshold
  datatools::logger::priority get_logging_priority() const;

  /// Basic OCD support shared by all inherited modules
  static void common_ocd(datatools::object_configuration_description & ocd_);

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog,
                         const std::string & title = "",
                         const std::string & indent = "",
                         bool inherit = false) const;

 protected:

  /// Common initialization of services
  void common_initialize(const datatools::properties & config);

 protected:

  datatools::logger::priority _logging_priority; //!< Logging priority threshold
  std::string name_;         //!< The name of the service
  std::string description_;  //!< The description of the service
  std::string version_;      //!< The version of the service

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_service);

};

}  // end of namespace datatools

#define DATATOOLS_SERVICE_REGISTRATION_INTERFACE(SERVICE_CLASS_NAME)	\
  private:								\
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::datatools::base_service, SERVICE_CLASS_NAME); \
  /**/

#define DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(SERVICE_CLASS_NAME,SERVICE_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::datatools::base_service,SERVICE_CLASS_NAME,SERVICE_ID); \
  /**/


#endif // DATATOOLS_BASE_SERVICE_H
