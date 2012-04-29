// -*- mode: c++ ; -*- 
/* placement_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2010-02-13
 * 
 * License: 
 * 
 * Description: 
 *
 *   Vertex generator with placement.
 * 
 * History: 
 * 
 */

#ifndef __genvtx__placement_vg_h
#define __genvtx__placement_vg_h 1

#include <genvtx/i_vertex_generator.h>
#include <geomtools/placement.h>

namespace genvtx {
  
  GENVTX_VG_CLASS_DECLARE(placement_vg)
  {
  public: 
    
    const geomtools::placement & get_placement () const;
    
    geomtools::placement & get_placement ();
    
    void set_placement (const geomtools::placement &);
    
    bool has_vg () const;
    
    bool is_owned () const;
    
    const i_vertex_generator & get_vg () const;
   
    i_vertex_generator & grab_vg ();
    
    void set_vg (i_vertex_generator &);
    
    void set_vg (i_vertex_generator *);
    
    void set_vg (vg_handle_type &);
    
    void _clear_vg_ ();
    
  public: 
    
    GENVTX_VG_INTERFACE_CTOR_DTOR(placement_vg);
    
    GENVTX_VG_HAS_NEXT_VERTEX_DECLARE();
    
  public:

    placement_vg (const i_vertex_generator &, 
                  const geomtools::placement &);
    
  private: 
    
    geomtools::placement _placement_;
    bool                 _owned_;
    i_vertex_generator * _vg_;
    vg_handle_type       _hvg_;
    
  private:
    
    GENVTX_VG_REGISTRATION_INTERFACE(placement_vg);

  };

} // end of namespace genvtx

#endif // __genvtx__placement_vg_h

// end of placement_vg.h
