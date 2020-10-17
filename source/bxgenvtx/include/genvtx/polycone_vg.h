/// \file genvtx/polycone_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-10-11
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *   Polycone vertex generator
 *   Generation of vertex in a 3D polycone
 *
 * History:
 *
 */

#ifndef GENVTX_POLYCONE_VG_H
#define GENVTX_POLYCONE_VG_H 1

// Standard library
#include <iostream>
#include <vector>

// Third party
// - geomtools
#include <geomtools/polycone.h>

// This project
#include <genvtx/i_vertex_generator.h>

namespace datatools {
  class properties;
}

namespace genvtx {

  /// \brief A vertex generator based on the geometry of a 3D polycone
  class polycone_vg : public i_vertex_generator
  {
  public:

    static const int MODE_INVALID = -1;
    static const int MODE_BULK    =  0;
    static const int MODE_SURFACE =  1;
    static const int MODE_DEFAULT = MODE_BULK;

    enum bulk_bits {
      BULK_NONE     = 0,
      BULK_BODY     = datatools::bit_mask::bit00,
      BULK_CAVITY   = datatools::bit_mask::bit01,
      BULK_ALL      = (BULK_BODY | BULK_CAVITY)
    };

    int get_mode() const;

    void set_mode(int mode_);

    void set_surface_mask(int surface_mask_);

    void set_bulk_mask(int bulk_mask_);

    void set_angles(double theta_min_, double theta_max_);

    void set_skin_skip(double skin_skip_);

    void set_skin_thickness(double skin_thickness_);

    void set_bulk(int bulk_mask_, double skin_thickness_);

    void set_surface(int surface_mask_);

    void set_active_frustrum(size_t index_, bool active_);

    void set_active_all_frustrum();

    void set_polycone(const geomtools::polycone & polycone_);

    const geomtools::polycone & get_polycone() const;

    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;


    /// Constructor
    polycone_vg();

    /// Destructor
    ~polycone_vg() override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &) override;

    /// Reset
    void reset() override;

  protected :

    /// Randomize vertex
    void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_) override;

  private:

    void _init_();

    void _reset_();

    void _set_defaults_();

  private:

    bool           _initialized_;
    geomtools::polycone _polycone_;
    int            _mode_;
    int            _surface_mask_;
    int            _bulk_mask_;
    double         _skin_skip_;
    double         _skin_thickness_;
    double         _theta_min_;
    double         _theta_max_;
    double         _sum_surface_weight_[4];
    double         _sum_bulk_weight_[2];
    std::vector<bool>   _active_frustra_;
    std::vector<double> _volume_inner_frustra_;
    std::vector<double> _volume_frustra_;
    std::vector<double> _surface_inner_frustra_;
    std::vector<double> _surface_outer_frustra_;
    double _surface_bottom_;
    double _surface_top_;

    /// Registration macro
    /// @arg polycone_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(polycone_vg)

  };

} // end of namespace genvtx

#endif // GENVTX_POLYCONE_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
