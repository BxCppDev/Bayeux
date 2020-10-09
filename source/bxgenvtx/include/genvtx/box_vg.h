/// \file genvtx/box_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *   Box vertex generator
 *   Generation of vertex in a 3D box
 *
 * History:
 *
 */

#ifndef GENVTX_BOX_VG_H
#define GENVTX_BOX_VG_H 1

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/geomtools
#include <geomtools/box.h>

// This project:
#include <genvtx/i_vertex_generator.h>

namespace geomtools {
  // Forward declaration:
  class logical_volume;
}

namespace genvtx {

  /// \brief A vertex generator based on the geometry of a 3D box
  class box_vg : public i_vertex_generator
  {
  public:

    static const int MODE_INVALID = -1;
    static const int MODE_BULK    =  0;
    static const int MODE_SURFACE =  1;
    static const int MODE_DEFAULT = MODE_BULK;

  public:

    /// Return mode
    int get_mode () const;

    /// Set mode
    void set_mode (int mode_);

    /// Set the surface mask
    void set_surface_mask (int surface_mask_);

    /// Set the skin skip distance
    void set_skin_skip (double skin_skip_);

    /// Set the skin thickness
    void set_skin_thickness (double skin_thickness_);

    /// Set bulk mode
    void set_bulk (double skin_thickness_);

    /// Set surface mode
    void set_surface (int surface_mask_);

    /// Set the embedded cylinder
    void set_box (const geomtools::box & box_);

    /// Return a const referece to the embedded cylinder
    const geomtools::box & get_box () const;

    /// Check if a referenced cylinder is available
    bool has_box_ref () const;

    /// Set the reference to an external cylinder
    void set_box_ref (const geomtools::box & box_);

    /// Return a const reference to the referenced external cylinder
    const geomtools::box & get_box_ref () const;

    /// Check if a safe referenced cylinder is available
     bool has_box_safe() const;

    /// Return a safe const reference to the cylinder
    const geomtools::box & get_box_safe () const;

    /// Check if a referenced logical is available
    bool has_logical() const;

    /// Set the reference to an external logical
    void set_logical(const geomtools::logical_volume &);

    /// Reset the reference to an external logical
    void reset_logical();

    /// Smart print
    void tree_dump (std::ostream & out_ = std::clog,
                    const std::string & title_ = "",
                    const std::string & indent_ = "",
                    bool inherit_ = false) const override;

    /// Constructor
    box_vg();

    /// Destructor
    ~box_vg() override;

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

    /// Private initialization
    void _init_();

    /// Private reset
     void _reset_();

     /// Set the default attributes' values
   void _set_defaults_();

  private:

    bool                   _initialized_; //!< Initialization flag
    const geomtools::logical_volume * _log_vol_; //!< External logical volume handle
    geomtools::box         _box_;         //!< Embedded box object
    const geomtools::box * _box_ref_;     //!< External box object handle

    int            _mode_;         //!< Vertex randomization mode (bulk/surface)
    int            _surface_mask_; //!< Surface mask
    double         _skin_skip_;    //!< Skip (normal to the surface) to an effective position of the skin relative to the surface of the box
    double         _skin_thickness_;   //!< Intrinsic thickness of the surface
    double         _sum_weight_[6];    //!< Probability weights for surface randomization

  private:

    /// Registration macro
    /// @arg box_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(box_vg)

  };

} // end of namespace genvtx

#endif // GENVTX_BOX_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
