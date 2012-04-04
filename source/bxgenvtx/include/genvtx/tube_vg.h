// -*- mode: c++ ; -*- 
/* tube_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-04
 * Last modified: 2012-04-04
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

#ifndef __genvtx__tube_vg_h
#define __genvtx__tube_vg_h 1

#include <iostream>

#include <genvtx/i_vertex_generator.h>
#include <geomtools/tube.h>
#include <geomtools/cylinder.h>

namespace datatools {
namespace utils {
  class properties;
}
}

namespace genvtx {
  
  class tube_vg : public i_vertex_generator
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

    void _assert_lock_ (const std::string & where_);

  public: 

    bool is_initialized () const;

    int get_mode () const;

    void set_mode (int mode_);

    void set_surface_mask (int surface_mask_);

    void set_skin_skip (double skin_skip_);

    void set_skin_thickness (double skin_thickness_);

    void set_bulk (double skin_thickness_);

    void set_surface (int surface_mask_);

    void set_tube (const geomtools::tube & tube_);

    const geomtools::tube & get_tube () const;

    void dump (std::ostream & out_ = std::clog) const;

    // ctor:
    tube_vg ();

    // dtor:
    virtual ~tube_vg ();

    void init ();

    void reset ();

  private:

    void _init_ ();

    void _reset_ ();

  public:

    virtual std::string vg_id () const;

    virtual vg_creator_type vg_creator () const;

    static i_vertex_generator * create (const datatools::utils::properties & configuration_, void * user_ = 0);
  
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
                                geomtools::vector_3d & vertex_);
    
  private:
    bool           _initialized_;
    geomtools::tube _tube_;
    int            _mode_;
    int            _surface_mask_;
    double         _skin_skip_;
    double         _skin_thickness_;
    double         _sum_weight_[4];
    // double         _r1_sqr_;
    // double         _r2_sqr_;

 
  private:

    static creator_registration<tube_vg> g_cr_;
  
  };

} // end of namespace genvtx

#endif // __genvtx__tube_vg_h

// end of tube_vg.h
