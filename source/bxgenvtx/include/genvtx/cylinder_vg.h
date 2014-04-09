/// \file genvtx/cylinder_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2010-02-13
 *
 * License:
 *
 * Description:
 *   Cylinder vertex generator
 *   Generation of vertex in a 3D cylinder
 *
 * History:
 *
 */

#ifndef GENVTX_CYLINDER_VG_H
#define GENVTX_CYLINDER_VG_H 1

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/geomtools
#include <geomtools/cylinder.h>

// This project:
#include <genvtx/i_vertex_generator.h>

namespace datatools {
  class properties;
}

namespace genvtx {

  /// \brief A vertex generator based on the geometry of a 3D cylinder
  class cylinder_vg : public i_vertex_generator
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

    void set_cylinder (const geomtools::cylinder & cylinder_);

    const geomtools::cylinder & get_cylinder () const;

    void tree_dump (std::ostream & out_ = std::clog,
                    const std::string & title_ = "",
                    const std::string & indent_ = "",
                    bool inherit_ = false) const;

    /// Constructor
    cylinder_vg();

    /// Destructor
    virtual ~cylinder_vg();

    /// Initialization
    virtual void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &);

    /// Reset
    virtual void reset();

    /// Check initialization status
    virtual bool is_initialized() const;

  protected :

    /// Randomize vertex
    virtual void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  private:

    bool           _initialized_; /// Initialization flag
    geomtools::cylinder _cylinder_; /// Embedded cylinder object
    int            _mode_; /// Vertex randomization mode (bulk/surface)
    int            _surface_mask_; /// Surface mask
    double         _skin_skip_; /// Skip (normal to the surface) to an effective position of the skin relative to the surface of the cylinder
    double         _skin_thickness_; /// Intrinsic thickness of the surface
    double         _sum_weight_[3];/// Probability weights for surface randomization

    /// Registration macro
    /// @arg cylinder_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(cylinder_vg);

  };

} // end of namespace genvtx

#endif // GENVTX_CYLINDER_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
