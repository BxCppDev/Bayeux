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

namespace geomtools {
  // Forward declaration:
  class logical_volume;
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

    bool has_tube_ref () const;

    void set_tube_ref (const geomtools::tube & tube_);

    const geomtools::tube & get_tube_ref () const;

    bool has_tube_safe() const;

    const geomtools::tube & get_tube_safe () const;

     /// Check logical
    bool has_logical() const;

     /// Set logical
    void set_logical(const geomtools::logical_volume &);

     /// Reset logical
    void reset_logical();

     /// Smart print
    void tree_dump (std::ostream & out_ = std::clog,
                    const std::string & title_ = "",
                    const std::string & indent_ = "",
                    bool inherit_ = false) const override;

    /// Constructor
    tube_vg();

    /// Destructor
    ~tube_vg() override;

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

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  private:

    bool            _initialized_; /// Initialization flag
    const geomtools::logical_volume * _log_vol_; //!< External logical volume handle
    const geomtools::tube * _tube_ref_;  //!< External tube object handle
    geomtools::tube _tube_; //!< Embedded tube object
    int             _mode_; //!< Vertex randomization mode (bulk/surface)
    int             _surface_mask_; //!< Surface mask
    double          _skin_skip_; //!< Skip (normal to the surface) to an effective position of the skin relative to the surface of the tube
    double          _skin_thickness_; //!< Intrinsic thickness of the surface
    double          _sum_weight_[4]; //!< Probability weights for surface randomization

    /// Registration macro
    /// @arg tube_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(tube_vg)

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
