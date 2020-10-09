/// \file genvtx/cylinder_model_vg.h
/*  Author(s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-13
 * Last modified: 2013-03-10
 *
 * License:
 *
 * Description:
 *
 *   Vertex generator from a cylinder model addressed through some
 *   collection of geometry IDs extracted from a mapping object.
 *
 * History:
 *
 */

#ifndef GENVTX_CYLINDER_MODEL_VG_H
#define GENVTX_CYLINDER_MODEL_VG_H 1

// Standard library:
#include <string>

// Third party:
// // - Bayeux/geomtools
// #include <geomtools/id_selector.h>

// This project:
#include <genvtx/cylinder_vg.h>
#include <genvtx/vg_macros.h>
#include <genvtx/utils.h>
#include <genvtx/i_from_model_vg.h>

namespace genvtx {

  /// \brief A vertex generator based on a cylindric geometry model
  class cylinder_model_vg : public i_from_model_vg
  {
  public:

    bool is_mode_valid () const;

    bool is_mode_bulk () const;

    bool is_mode_surface () const;

    bool is_surface_side () const;

    bool is_surface_bottom () const;

    bool is_surface_top () const;

    void set_surface_side (bool);

    void set_surface_bottom (bool);

    void set_surface_top (bool);

    void set_skin_skip (double skin_skip_);

    void set_skin_thickness (double skin_thickness_);

    int get_mode () const;

    void set_mode (int);

    void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_ = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const override;

    /// Constructor
    cylinder_model_vg();

    /// Destructor
    ~cylinder_model_vg() override;

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

    void _shoot_vertex_cylinders (mygsl::rng & random_,
                                  geomtools::vector_3d & vertex_);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  private:

    bool                    _initialized_;    //!< Initialization flag
    int                     _mode_;           //!< Mode : "bulk" of "surface"
    bool                    _surface_side_;   //!< Flag for side surface generation mode
    bool                    _surface_bottom_; //!< Flag for bottom surface generation mode
    bool                    _surface_top_;    //!< Flag for top surface generation mode
    double                  _skin_skip_;      //!< Skip (normal to the surface) to an effective position of the skin relative to the surface of the box
    double                  _skin_thickness_; //!< Intrinsic thickness of the surface
    genvtx::cylinder_vg     _cylinder_vg_;    //!< Embeded vertex generator from a cylinder
    std::vector<weight_entry_type> _entries_; //!< Information about the weights

    /// Registration macro
    /// @arg cylinder_model_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(cylinder_model_vg)

  };

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genvtx::cylinder_model_vg)

#endif // GENVTX_CYLINDER_MODEL_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
