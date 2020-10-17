/// \file genvtx/sphere_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-10-10
 * Last modified: 2014-04-09
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

#ifndef GENVTX_SPHERE_VG_H
#define GENVTX_SPHERE_VG_H 1

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/geomtools
#include <geomtools/sphere.h>

// This project:
#include <genvtx/i_vertex_generator.h>

namespace geomtools {
  // Forward declaration:
  class logical_volume;
}

namespace genvtx {

  /// \brief Vertex generator from a sphere
  class sphere_vg : public i_vertex_generator
  {
  public:

    static const int MODE_INVALID = -1;
    static const int MODE_BULK    =  0;
    static const int MODE_SURFACE =  1;
    static const int MODE_DEFAULT = MODE_BULK;

    int get_mode () const;

    void set_mode (int mode_);

    void set_surface_mask (int surface_mask_);

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

    /// Check if a referenced logical is available
    bool has_logical() const;

    /// Set the reference to an external logical
    void set_logical(const geomtools::logical_volume &);

    /// Reset the reference to an external logical
    void reset_logical();

    void tree_dump (std::ostream & out_ = std::clog,
                    const std::string & title_ = "",
                    const std::string & indent_ = "",
                    bool inherit_ = false) const override;

    /// Constructor
    sphere_vg();

    /// Destructor
    ~sphere_vg() override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &) override;

    /// Reset
    void reset() override;

    /// Check initialization status
    bool is_initialized() const override;

  protected :

    /// Randomize vertex
    void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_) override;

  private:

    /// Internal initialization
    void _init_ ();

    /// Internal reset
    void _reset_ ();

    /// Set default attributes
    void _set_defaults_ ();

  private:

    bool                      _initialized_; /// Initialization flag
    const geomtools::logical_volume * _log_vol_; //!< External logical volume handle
    geomtools::sphere         _sphere_;      /// Embedded sphere object
    const geomtools::sphere * _sphere_ref_;  /// External sphere object handle

    int            _mode_;          /// Vertex randomization mode (bulk/surface)
    int            _surface_mask_;  /// Surface mask
    double         _skin_skip_;     /// Skip (normal to the surface) to an effective position of the skin relative to the surface of the sphere
    double         _skin_thickness_; /// Intrinsic thickness of the surface
    double         _sum_weight_[6];

    /// Registration macro
    /// @arg sphere_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(sphere_vg)

  };

  void randomize_sphere(mygsl::rng & random_,
                        double r1_, double r2_,
                        double theta1_, double theta2_,
                        double phi1_, double phi2_,
                        geomtools::vector_3d & vertex_);

} // end of namespace genvtx

#endif // GENVTX_SPHERE_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
