/// \file geomtools/cone.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-10-11
 * Last modified: 2015-03-12
 *
 * License:
 *
 * Description:
 *
 *   Cone utilities and shapes
 *
 */

#ifndef GEOMTOOLS_CONE_H
#define GEOMTOOLS_CONE_H 1

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  // Class forward declarations:
  class disk;
  class quadrangle;

  /// \brief Cone utilities
  class cone {
  public:

    /// Compute the lateral surface of a conical frustrum
    static double compute_frustrum_lateral_surface(double z1_, double z2_,
                                                   double r1_, double r2_);

    /// Compute the volume of a conical frustrum
    static double compute_frustrum_volume(double z1_, double z2_,
                                          double r1_, double r2_);


    /// Generate a sequence of polylines for wires 3D rendering
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_CONE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
