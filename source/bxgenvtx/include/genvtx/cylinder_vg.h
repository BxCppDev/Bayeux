// -*- mode: c++ ; -*- 
/* cylinder_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GENVTX_CYLINDER_VG_H_
#define GENVTX_CYLINDER_VG_H_ 1

#include <iostream>

#include <genvtx/i_vertex_generator.h>
#include <geomtools/cylinder.h>

namespace datatools {
  class properties;
}

namespace genvtx {
  
  GENVTX_VG_CLASS_DECLARE(cylinder_vg)
  {
  public: 

    static const int MODE_INVALID = -1;
    static const int MODE_BULK    =  0;
    static const int MODE_SURFACE =  1;
    static const int MODE_DEFAULT = MODE_BULK;

  public: 

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
      
    GENVTX_VG_INTERFACE_CTOR_DTOR (cylinder_vg);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  private:

    bool           _initialized_;
    geomtools::cylinder _cylinder_;
    int            _mode_;
    int            _surface_mask_;
    double         _skin_skip_;
    double         _skin_thickness_;
    double         _sum_weight_[3];

    GENVTX_VG_REGISTRATION_INTERFACE(cylinder_vg);
  
  };

} // end of namespace genvtx

#endif // GENVTX_CYLINDER_VG_H_

// end of cylinder_vg.h
