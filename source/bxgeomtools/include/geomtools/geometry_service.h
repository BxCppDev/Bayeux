/// \file geomtools/geometry_service.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-12-14
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Generic geometry service.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_GEOMETRY_SERVICE_H
#define GEOMTOOLS_GEOMETRY_SERVICE_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party :
// - Bayeux/datatools:
#include <datatools/base_service.h>
#include <datatools/service_tools.h>

namespace datatools {
  class properties;
}

namespace geomtools {

  class manager;

  /// \brief Generic geometry service
  class geometry_service : public datatools::base_service
  {

  public:

    /// Check initialization flag
    bool is_initialized () const override;

    /// Initialization
    int initialize (const datatools::properties & a_config,
                            datatools::service_dict_type & a_service_dict) override;

    /// Termination
    int reset () override;

    /// Return a reference to the non mutable geoemtry manager
    const geomtools::manager & get_geom_manager () const;

    /// Default constructor
    geometry_service ();

    /// Destructor
    ~geometry_service () override;

    /// Smart print
    void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit               = false) const override;

  private:

    bool                 _initialized_; //!< Initialization flag
    geomtools::manager * _geom_manager_; //!< Handle to the geometry manager

    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(geometry_service)

  };

}  // end of namespace geomtools

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(geomtools::geometry_service)

#endif // GEOMTOOLS_GEOMETRY_SERVICE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
