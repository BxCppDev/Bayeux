//! \file  mctools/mcnp/geometry/surface_type.h
//! \brief MNCP surface types
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MCTOOLS_MCNP_GEOMETRY_SURFACE_TYPE_H
#define MCTOOLS_MCNP_GEOMETRY_SURFACE_TYPE_H

// Standard library:
#include <string>
#include <list>

// Third party:
// - Boost:
#include <boost/tuple/tuple.hpp>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      //! \brief Type of surface
      enum surface_type {
        SURF_INVALID         = -1,
        SURF_PLANE           =  0,
        SURF_PLANE_X         =  1,
        SURF_PLANE_Y         =  2,
        SURF_PLANE_Z         =  3,
        SURF_GENERAL_SPHERE  =  4,
        SURF_SPHERE_O        =  5,
        SURF_SPHERE_X        =  6,
        SURF_SPHERE_Y        =  7,
        SURF_SPHERE_Z        =  8,
        SURF_CYL_PARAL_X     =  9,
        SURF_CYL_PARAL_Y     = 10,
        SURF_CYL_PARAL_Z     = 11,
        SURF_CYL_AXIS_X      = 12,
        SURF_CYL_AXIS_Y      = 13,
        SURF_CYL_AXIS_Z      = 14,
        SURF_CONE_PARAL_X    = 15,
        SURF_CONE_PARAL_Y    = 16,
        SURF_CONE_PARAL_Z    = 17,
        SURF_CONE_AXIS_X     = 18,
        SURF_CONE_AXIS_Y     = 19,
        SURF_CONE_AXIS_Z     = 20,
        SURF_SPECIAL_QUADRIC = 21,
        SURF_GENERAL_QUADRIC = 22,
        SURF_TORUS_PARAL_X   = 23,
        SURF_TORUS_PARAL_Y   = 24,
        SURF_TORUS_PARAL_Z   = 25,
        SURF_BY_POINTS       = 26
      };

      //! Return the MCNP label associated to a given surface type
      std::string surface_type_to_mcnp_label(surface_type);

      //! Return the MCNP surface name associated to a given surface type
      std::string surface_type_to_mcnp_description(surface_type);

      typedef boost::tuple<std::string, std::string> param_desc_type;

      typedef std::list<param_desc_type> expected_parameters_type;

      // expected_parameters_type


    } // namespace geometry

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_GEOMETRY_SURFACE_TYPE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
