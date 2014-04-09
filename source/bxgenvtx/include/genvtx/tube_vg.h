/// \file genvtx/tube_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-04
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *   Tube vertex generator
 *   Generation of vertex in a 3D tube
 *
 * History:
 *
 */

#ifndef GENVTX_TUBE_VG_H
#define GENVTX_TUBE_VG_H 1

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/geomtools
#include <geomtools/tube.h>
#include <geomtools/cylinder.h>

// This project:
#include <genvtx/i_vertex_generator.h>

namespace datatools {
  class properties;
}

namespace genvtx {

  /// \brief A vertex generator based on the geometry of a 3D tube
  class tube_vg : public i_vertex_generator
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

    void set_tube (const geomtools::tube & tube_);

    const geomtools::tube & get_tube () const;

    void tree_dump (std::ostream & out_ = std::clog,
                    const std::string & title_ = "",
                    const std::string & indent_ = "",
                    bool inherit_ = false) const;

    /// Constructor
    tube_vg();

    /// Destructor
    virtual ~tube_vg();

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

    bool            _initialized_;
    geomtools::tube _tube_;
    int             _mode_;
    int             _surface_mask_;
    double          _skin_skip_;
    double          _skin_thickness_;
    double          _sum_weight_[4];

    /// Registration macro
    /// @arg tube_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(tube_vg);

  };

} // end of namespace genvtx

#endif // GENVTX_TUBE_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
