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

#ifndef __genvtx__cylinder_vg_h
#define __genvtx__cylinder_vg_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

#include <genvtx/i_vertex_generator.h>
#include <geomtools/cylinder.h>

namespace genvtx {
  
  class cylinder_vg : public i_vertex_generator
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
    geomtools::cylinder __cylinder;
    int            __mode;
    int            __surface_mask;
    double         __skin_skip;
    double         __skin_thickness;
    double         __sum_weight[3];

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

    void set_cylinder (const geomtools::cylinder & cylinder_);

    const geomtools::cylinder & get_cylinder () const;

    void dump (ostream & out_ = clog) const;

    // ctor:
    cylinder_vg ();

    // dtor:
    virtual ~cylinder_vg ();

    void init ();

    void reset ();

  private:

    void __init ();

    void __reset ();

  public:

    virtual string vg_id () const;

    virtual vg_creator_t vg_creator () const;

    static i_vertex_generator * create (const properties & configuration_);
  
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
				geomtools::vector_3d & vertex_);
 
  private:

    static creator_registration<cylinder_vg> __CR;
  

  };

} // end of namespace genvtx

#endif // __genvtx__cylinder_vg_h

// end of cylinder_vg.h
