// -*- mode: c++ ; -*- 
/* placement_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2010-02-13
 * 
 * License: 
 * 
 * Description: 
 *   Vertex generator with placement
 * 
 * History: 
 * 
 */

#ifndef __genvtx__placement_vg_h
#define __genvtx__placement_vg_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <genvtx/i_vertex_generator.h>
#include <geomtools/placement.h>

namespace genvtx {

  class placement_vg : public i_vertex_generator
  {
  private: 
    geomtools::placement __placement;
    bool                 __owned;
    i_vertex_generator * __vg;

  public: 

    const geomtools::placement & get_placement () const;

    geomtools::placement & get_placement ();

    void set_placement (const geomtools::placement &);

    bool has_vg () const;

    bool is_owned () const;

    const i_vertex_generator & get_vg () const;

    void set_vg (i_vertex_generator &);

    void set_vg (i_vertex_generator *);
  
  private:
    void __clear_vg ();

  // ctor/dtor:
  public: 

    placement_vg ();

    placement_vg (const i_vertex_generator &, 
		  const geomtools::placement &);

    virtual ~placement_vg ();
  
  public:

    virtual string vg_id () const;

    virtual vg_creator_t vg_creator () const;

    static i_vertex_generator * create (const properties & configuration_);
  
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
				geomtools::vector_3d & vertex_);
 
  private:

    static creator_registration<placement_vg> __CR;
  

  };

} // end of namespace genvtx

#endif // __genvtx__placement_vg_h

// end of placement_vg.h
