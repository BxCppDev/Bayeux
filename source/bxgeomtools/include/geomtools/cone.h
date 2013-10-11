// -*- mode: c++ ; -*-
/* cone.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-10-11
 * Last modified: 2013-10-11
 *
 * License:
 *
 * Description:
 *   Cone utilities
 *
 */

#ifndef GEOMTOOLS_CONE_H_
#define GEOMTOOLS_CONE_H_ 1

namespace geomtools {

  class cone {
  public:

    /// Compute the lateral surface of a conical frustrum
    static double compute_frustrum_lateral_surface(double z1_, double z2_,
                                                   double r1_, double r2_);

    /// Compute the volume of a conical frustrum
    static double compute_frustrum_volume(double z1_, double z2_,
                                          double r1_, double r2_);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_CONE_H_

// end of cone.h
