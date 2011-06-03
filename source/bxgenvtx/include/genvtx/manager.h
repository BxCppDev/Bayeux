// -*- mode: c++ ; -*- 
/* manager.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2010-02-12
 * 
 * License: 
 * 
 * Description: 
 *   SuperNEMO vertex generator manager
 * 
 * History: 
 * 
 */

#ifndef __genvtx__manager_h
#define __genvtx__manager_h 1

#include <string>

#include <mygsl/rng.h>

#include <datatools/utils/properties.h>
#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  using namespace std;

  class manager
  {
  private: 
    bool              __debug;
    bool              __initialized;
    unsigned long int __rng_seed;
    string            __rng_id; 
    mygsl::rng        __random;

  public: 
    bool is_debug () const;
    void set_debug (bool);
    bool is_initialized () const;
    const string & get_rng_id () const;
  
  public: 

    // ctor:
    manager ();

    // dtor:
    virtual ~manager ();
  
    void initialize ();
  
    void reset ();

    void shoot_vertex (geomtools::vector_3d & vertex_);
 
  };

} // end of namespace genvtx

#endif // __genvtx__manager_h

// end of manager.h
