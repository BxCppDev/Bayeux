// -*- mode: c++ ; -*-
/* sphere_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-10-10
 * Last modified: 2013-10-10
 *
 * License:
 *
 * Description:
 *   Sphere vertex generator
 *   Generation of vertex in a 3D sphere
 *
 * History:
 *
 */

#ifndef GENVTX_SPHERE_VG_H_
#define GENVTX_SPHERE_VG_H_ 1

#include <iostream>

#include <genvtx/i_vertex_generator.h>

#include <geomtools/sphere.h>

namespace genvtx {

  /// \brief Vertex generator fro a sphere
  GENVTX_VG_CLASS_DECLARE(sphere_vg)
  {
  public:

    static const int MODE_INVALID = -1;
    static const int MODE_BULK    =  0;
    static const int MODE_SURFACE =  1;
    static const int MODE_DEFAULT = MODE_BULK;

  public:

    int get_mode () const;

    void set_mode (int mode_);

    void set_skin_skip (double skin_skip_);

    void set_skin_thickness (double skin_thickness_);

    void set_bulk (double skin_thickness_);

    void set_surface (int surface_mask_);

    void set_sphere (const geomtools::sphere & sphere_);

    bool has_sphere_ref () const;

    void set_sphere_ref (const geomtools::sphere & sphere_);

    const geomtools::sphere & get_sphere () const;

    const geomtools::sphere & get_sphere_ref () const;

    bool has_sphere_safe () const;

    const geomtools::sphere & get_sphere_safe () const;

    void tree_dump (std::ostream & out_ = std::clog,
                    const std::string & title_ = "",
                    const std::string & indent_ = "",
                    bool inherit_ = false) const;

    GENVTX_VG_INTERFACE_CTOR_DTOR (sphere_vg);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  private:

    bool                      _initialized_; /// Initialization flag
    geomtools::sphere         _sphere_;      /// Embedded sphere object
    const geomtools::sphere * _sphere_ref_;  /// External sphere object handle

    int            _mode_;         /// Vertex randomization mode (bulk/surface)
    double         _skin_skip_;    /// Skip (normal to the surface) to an effective position of the skin relative to the surface of the sphere
    double         _skin_thickness_; /// Intrinsic thickness of the surface

  private:

    GENVTX_VG_REGISTRATION_INTERFACE(sphere_vg);

  };

  void randomize_sphere(mygsl::rng & random_,
                        double r1_, double r2_,
                        double theta1_, double theta2_,
                        double phi1_, double phi2_,
                        geomtools::vector_3d & vertex_);

} // end of namespace genvtx

#endif // GENVTX_SPHERE_VG_H_

// end of sphere_vg.h
