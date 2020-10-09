/// \file datatools/base_service.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2017-08-30
 *
 * Copyright (C) 2011-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

#ifndef DATATOOLS_BASE_SERVICE_H
#define DATATOOLS_BASE_SERVICE_H

// Standard Library
#include <iostream>
#include <map>
#include <stdexcept>
#include <typeinfo>
#include <vector>

// Third Party
#include <boost/function/function_base.hpp>

// This project (Bayeux/datatools)
#include <datatools/enriched_base.h>
#include <datatools/i_tree_dump.h>
#include <datatools/service_tools.h>
#include <datatools/factory_macros.h>
#include <datatools/exception.h>
#include "datatools/factory-inl.h"

#include <datatools/logger.h>
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

/// \brief Top-level namespace of the Bayeux/datatools module library
namespace datatools {

  // Forward class declaration:
  class properties;
  class object_configuration_description;

  /*! \brief The base service class
   *
   *  The base_service class provides the interface of all
   *  datatools service objects.
   */
  class base_service
    : public datatools::enriched_base
  {
  public:

    /// Default constructor
    base_service();

    /// Constructor
    base_service(const std::string & name_,
                 const std::string & display_name_ = "",
                 const std::string & description_ = "",
                 logger::priority lp_ = logger::PRIO_FATAL);

    /// Destructor
    ~base_service() override;

    /// Undocumented unused/unimplemented method
    virtual void fetch_dependencies(service_dependency_dict_type & /*dependencies_*/) const;

    /// Initialize the service without a list of properties nor other services
    virtual int initialize_simple();

    /// Initialize the service using only a list of properties without the needs of other services
    virtual int initialize_standalone(const datatools::properties & config_);

    /// Initialize the service using with access to a dictionary of other services
    virtual int initialize_with_services(service_dict_type & service_dict_);

    /// Initialize the service using a list of properties with access to a dictionary of other services
    virtual int initialize(const datatools::properties & config_,
                           service_dict_type & service_dict_) = 0;

    /// Reset the service
    virtual int reset() = 0;

    /// Check if service is initialized
    virtual bool is_initialized() const = 0;

    /// Return the service class
    virtual std::string service_class_id() const = 0;

    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;

    /// Smart print
    /// @deprecated
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;

    /// Basic OCD support shared by all inherited modules
    static void common_ocd(datatools::object_configuration_description & ocd_);

  protected:

    /// Common initialization of services
    void _common_initialize(const datatools::properties & config_);

    /// Common initialization of services
    /// \deprecated
    void common_initialize(const datatools::properties & config_);

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_service)

#ifndef Q_MOC_RUN
    //! Reflection interface
    DR_CLASS_RTTI()
#endif // Q_MOC_RUN

  };

} // end of namespace datatools

#include <datatools/service_tools-inl.h>

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_CLASS_INIT(datatools::base_service)
#endif // Q_MOC_RUN

#define DATATOOLS_SERVICE_REGISTRATION_INTERFACE(SERVICE_CLASS_NAME)    \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::datatools::base_service, SERVICE_CLASS_NAME) \
  public:                                                               \
  std::string service_class_id() const override;                         \
  /**/

#define DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(SERVICE_CLASS_NAME,SERVICE_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::datatools::base_service,SERVICE_CLASS_NAME,SERVICE_ID) \
  std::string SERVICE_CLASS_NAME::service_class_id() const {return SERVICE_ID;}
/**/

#endif // DATATOOLS_BASE_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
