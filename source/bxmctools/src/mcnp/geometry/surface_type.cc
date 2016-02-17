//! \file  mctools/mcnp/geometry/surface_type.cc
//! \brief MNCP surface type
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

// Ourselves:
#include <mctools/mcnp/geometry/surface_type.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      std::string surface_type_to_mcnp_label(surface_type t_)
      {
        std::string label;
        switch(t_) {
        SURF_PLANE           : label = "P"; break;
        SURF_PLANE_X         : label = "PX"; break;
        SURF_PLANE_Y         : label = "PY"; break;
        SURF_PLANE_Z         : label = "PZ"; break;
        SURF_GENERAL_SPHERE  : label = "S"; break;
        SURF_SPHERE_O        : label = "SO"; break;
        SURF_SPHERE_X        : label = "SX"; break;
        SURF_SPHERE_Y        : label = "SY"; break;
        SURF_SPHERE_Z        : label = "SZ"; break;
        SURF_CYL_PARAL_X     : label = "C/X"; break;
        SURF_CYL_PARAL_Y     : label = "C/Y"; break;
        SURF_CYL_PARAL_Z     : label = "C/Z"; break;
        SURF_CYL_AXIS_X      : label = "CX"; break;
        SURF_CYL_AXIS_Y      : label = "CY"; break;
        SURF_CYL_AXIS_Z      : label = "CZ"; break;
        SURF_CONE_PARAL_X    : label = "K/X"; break;
        SURF_CONE_PARAL_Y    : label = "K/Y"; break;
        SURF_CONE_PARAL_Z    : label = "K/Z"; break;
        SURF_CONE_AXIS_X     : label = "KX"; break;
        SURF_CONE_AXIS_Y     : label = "KY"; break;
        SURF_CONE_AXIS_Z     : label = "KZ"; break;
        SURF_SPECIAL_QUADRIC : label = "SQ"; break;
        SURF_GENERAL_QUADRIC : label = "GQ"; break;
        SURF_TORUS_PARAL_X   : label = "TX"; break;
        SURF_TORUS_PARAL_Y   : label = "TY"; break;
        SURF_TORUS_PARAL_Z   : label = "TZ"; break;
        SURF_BY_POINTS       : label = "XYZP"; break;
        }
        return label;
      }

      std::string surface_type_to_mcnp_description(surface_type t_)
      {
        std::string name;
        switch(t_) {
        SURF_PLANE           : name = "General plane"; break;
        SURF_PLANE_X         : name = "Plane normal to X"; break;
        SURF_PLANE_Y         : name = "Plane normal to Y"; break;
        SURF_PLANE_Z         : name = "Plane normal to Z"; break;
        SURF_GENERAL_SPHERE  : name = "General sphere"; break;
        SURF_SPHERE_O        : name = "Sphere centered at origin"; break;
        SURF_SPHERE_X        : name = "Sphere centered along the X axis"; break;
        SURF_SPHERE_Y        : name = "Sphere centered along the Y axis"; break;
        SURF_SPHERE_Z        : name = "Sphere centered along the Z axis"; break;
        SURF_CYL_PARAL_X     : name = "Cylinder parallel to the X axis"; break;
        SURF_CYL_PARAL_Y     : name = "Cylinder parallel to the Y axis"; break;
        SURF_CYL_PARAL_Z     : name = "Cylinder parallel to the Z axis"; break;
        SURF_CYL_AXIS_X      : name = "Cylinder along the X axis"; break;
        SURF_CYL_AXIS_Y      : name = "Cylinder along the Y axis"; break;
        SURF_CYL_AXIS_Z      : name = "Cylinder along the Z axis"; break;
        SURF_CONE_PARAL_X    : name = "Cone parallel to the X axis"; break;
        SURF_CONE_PARAL_Y    : name = "Cone parallel to the Y axis"; break;
        SURF_CONE_PARAL_Z    : name = "Cone parallel to the Z axis"; break;
        SURF_CONE_AXIS_X     : name = "Cone along the X axis"; break;
        SURF_CONE_AXIS_Y     : name = "Cone along the Y axis"; break;
        SURF_CONE_AXIS_Z     : name = "Cone along the Z axis"; break;
        SURF_SPECIAL_QUADRIC : name = "Special ellipsoid/hyperboloid/paraboloid parallel to X/Y/Z axis"; break;
        SURF_GENERAL_QUADRIC : name = "General cylinder/cone/ellipsoid/hyperboloid/paraboloid not parallel to X/Y/Z axis"; break;
        SURF_TORUS_PARAL_X   : name = "Elliptical or circular torus parallel to X axis"; break;
        SURF_TORUS_PARAL_Y   : name = "Elliptical or circular torus parallel to Y axis"; break;
        SURF_TORUS_PARAL_Z   : name = "Elliptical or circular torus parallel to Z axis"; break;
        SURF_BY_POINTS       : name = "Surface defined by points"; break;
        }
        return name;
      }

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools
