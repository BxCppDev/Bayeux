// -*- mode: c++ ; -*- 
/* spot_vertex_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2010-02-12
 * 
 * License: 
 * 
 * Description: 
 *   Spot vertex generator
 * 
 * History: 
 * 
 */

#ifndef __genvtx__spot_vertex_generator_h
#define __genvtx__spot_vertex_generator_h 1

#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  class spot_vertex_generator : public i_vertex_generator
  {

  public: 
    const geomtools::vector_3d & get_spot () const;

    void set_spot (double x_, double y_, double z_);

    void set_spot (const geomtools::vector_3d &);
  
  public: 

    // ctor:
    spot_vertex_generator ();

    // ctor:
    spot_vertex_generator (double x_, double y_, double z_);

    // ctor:
    spot_vertex_generator (const geomtools::vector_3d & spot_);

    // dtor:
    virtual ~spot_vertex_generator ();
  
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
                                geomtools::vector_3d & vertex_);

  public:

    virtual string vg_id () const;

    virtual vg_creator_type vg_creator () const;

    static i_vertex_generator * create (const properties & configuration_, void * user_ = 0);

  private: 

    geomtools::vector_3d _spot_;
 
  private:

    static creator_registration<spot_vertex_generator> g_cr_;

  };

} // end of namespace genvtx

#endif // __genvtx__spot_vertex_generator_h

// end of spot_vertex_generator.h
