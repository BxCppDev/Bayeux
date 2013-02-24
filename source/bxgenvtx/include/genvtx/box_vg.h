// -*- mode: c++ ; -*- 
/* box_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2010-02-13
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

#ifndef GENVTX_BOX_VG_H_
#define GENVTX_BOX_VG_H_ 1

#include <iostream>

#include <genvtx/i_vertex_generator.h>

#include <geomtools/box.h>

namespace genvtx {
  
  GENVTX_VG_CLASS_DECLARE(box_vg)
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

    void set_box (const geomtools::box & box_);

    bool has_box_ref () const;

    void set_box_ref (const geomtools::box & box_);

    const geomtools::box & get_box () const;

    const geomtools::box & get_box_ref () const;

    bool has_box_safe () const;

    const geomtools::box & get_box_safe () const;

    void tree_dump (std::ostream & out_ = std::clog, 
                    const std::string & title_ = "", 
                    const std::string & indent_ = "", 
                    bool inherit_ = false) const;
      
    GENVTX_VG_INTERFACE_CTOR_DTOR (box_vg);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();
    
  private:

    bool                   _initialized_; /// Initialization flag
    geomtools::box         _box_;         /// Embedded box object
    const geomtools::box * _box_ref_;     /// External box object handle

    int            _mode_;         /// Vertex randomization mode (bulk/surface)
    int            _surface_mask_; /// Surface flag
    double         _skin_skip_;    /// Skip (normal to the surface) to an effective position of the skin relative to the surface of the box    
    double         _skin_thickness_;   /// Intrinsic thickness of the 
    double         _sum_weight_[6];    /// Probability weights for surface randomization
 
  private:

    GENVTX_VG_REGISTRATION_INTERFACE(box_vg);
  
  };

} // end of namespace genvtx

#endif // GENVTX_BOX_VG_H_

// end of box_vg.h
