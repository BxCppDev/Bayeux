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

#ifndef __genvtx__box_vg_h
#define __genvtx__box_vg_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

#include <genvtx/i_vertex_generator.h>

#include <geomtools/box.h>
#include <datatools/utils/units.h>

namespace genvtx {
  
  class box_vg : public i_vertex_generator
  {
  public: 
    static bool g_debug;

    enum mode_t
      {
	MODE_BULK = 0,
	MODE_SURFACE = 1,
	MODE_DEFAULT = MODE_BULK
      };
    
  private:
    bool           __initialized;
    geomtools::box __box;
    const geomtools::box * __box_ref;
    int            __mode;
    int            __surface_mask;
    double         __skin_skip;
    double         __skin_thickness;
    double         __sum_weight[6];

  private:    

    void __assert_lock (const string & where_);

  public: 

    bool is_initialized () const;

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

    const geomtools::box & get_box_safe () const;

    const geomtools::box & get_box () const;

    const geomtools::box & get_box_ref () const;

    void dump (ostream & out_ = clog) const;

    // ctor:
    box_vg ();

    // dtor:
    virtual ~box_vg ();

    void init ();

    void reset ();

  private:

    void __init ();

    void __reset ();

  public:

    virtual string vg_id () const;

    virtual vg_creator_t vg_creator () const;

    static i_vertex_generator * create (const properties & configuration_, 
					void * user_ = 0);
  
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
				geomtools::vector_3d & vertex_);
 
  private:

    static creator_registration<box_vg> __CR;
  
  };

} // end of namespace genvtx

#endif // __genvtx__box_vg_h

// end of box_vg.h
