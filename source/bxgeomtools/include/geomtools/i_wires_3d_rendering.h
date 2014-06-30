// -*- mode: c++ ; -*-
/// \file geomtools/i_wires_3d_rendering.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-10-22
 * Last modified: 2010-10-22
 *
 * License:
 *
 * Description:
 *   A abstract interface for wires 3D rendering
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_WIRES_3D_RENDERING_H
#define GEOMTOOLS_I_WIRES_3D_RENDERING_H 1

// Standard library:
#include <list>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  /// brief Abstract interface for object able to describe themselves as a list of polyline for 3D rendering
  struct i_wires_3d_rendering
  {
  public:

    virtual ~i_wires_3d_rendering ()
    {
      return;
    }

    virtual void generate_wires (std::list<polyline_3d> & wires_,
                                 const placement & positioning_,
                                 uint32_t options_) const = 0;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_WIRES_3D_RENDERING_H
