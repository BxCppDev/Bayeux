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
  public: 

    bool is_debug () const;

    void set_debug (bool);

    bool is_initialized () const;

    const string & get_rng_id () const;

    // ctor:
    manager ();

    // dtor:
    virtual ~manager ();
  
    void initialize ();
  
    void reset ();

    void shoot_vertex (geomtools::vector_3d & vertex_);
 
  private: 

    bool              _debug_;
    bool              _initialized_;
    unsigned long int _rng_seed_;
    string            _rng_id_; 
    mygsl::rng        _random_;

  };

} // end of namespace genvtx

#endif // __genvtx__manager_h

// end of manager.h
