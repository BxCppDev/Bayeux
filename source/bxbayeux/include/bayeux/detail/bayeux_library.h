// bayeux/detail/bayeux_library.h - Implementation of Bayeux Library System Singleton
//
// Copyright (c) 2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 Université de Caen
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

#ifndef BAYEUX_DETAIL_BAYEUX_LIBRARY_H
#define BAYEUX_DETAIL_BAYEUX_LIBRARY_H

// Standard library
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

// Third party:
#include <boost/core/noncopyable.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/service_manager.h>

namespace bayeux {

  namespace detail {

    // Singleton to automate Bayeux library embedded material (services...)
    class bayeux_library
      : private boost::noncopyable
    {
    public:

      /// Return the name of the Bayeux library's URN to resource path resolver service
      static const std::string & bx_resource_resolver_name();

      // Return the logging priority set from the environment
      static datatools::logger::priority process_logging_env();

      /// Default constructor
      bayeux_library();

      /// Destructor
      ~bayeux_library();

      /// Return the logging priority
      datatools::logger::priority get_logging() const;

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialize
      void initialize();

      /// Shutdown
      void shutdown();

      /// Check if the Bayeux system singleton is instantiated
      static bool is_instantiated();

      /// Return a mutable reference to the Bayeux system singleton instance
      static bayeux_library & instance();

      /// Return a non-mutable reference to the Bayeux system singleton instance
      static const bayeux_library & const_instance();

      /// Instantiate the Vire system singleton
      static bayeux_library & instantiate();

    private:

      void _libinfo_registration_();

      void _libinfo_deregistration_();

      void _initialize_urn_services_();

      void _shutdown_urn_services_();

    private:

      // Management:
      bool _initialized_ = false;
      datatools::logger::priority _logging_; //!< Logging priority threshold

      // Working internal data:
      datatools::service_manager _services_; //!< Embedded services

      // Singleton:
      static bayeux_library * _instance_; //!< Bayeux library system singleton handle

    };

  } // end of namespace detail

} // end of namespace bayeux

#endif // BAYEUX_DETAIL_BAYEUX_LIBRARY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
