// -*- mode: c++ ; -*- 
/* factory.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2010-02-12
 * 
 * License: 
 * 
 * Description: 
 *   Vertex generator factory: 
 *   The factory enables the dynamic creation of an
 *   arbitrary vertex generator from its ID (as registered in the
 *   vertex_generator_db static instance [see 'i_vertex_generator.h'])
 *   using some list of setup properties stored in some 'properties'
 *   object and some optional user data provided through a void * pointer
 *   in the 'create_vertex_generator' method.
 *
 * History: 
 * 
 */

#ifndef __genvtx__factory_h
#define __genvtx__factory_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  using namespace std;

  class factory
  {
  private: 
    bool                            __debug;
    vertex_generator_creator_dict_t __creators;

  public: 
    bool is_debug () const;
    void set_debug (bool);
  
  public: 
    // ctor:
    factory (bool preload_ = true);

    // dtor:
    virtual ~factory ();
 
    void do_register (const vg_creator_t & creator_, 
		      const string & vg_id_ = "");
    
    // from the i_vertex_generator:
    virtual i_vertex_generator * 
    create_vertex_generator (const string & vg_id_,
			     const datatools::utils::properties & vg_configuration_, 
			     void * user_ = 0);
    
    void dump_vertex_generators (ostream & out_ = clog);
    
  };

} // end of namespace genvtx

#endif // __genvtx__factory_h

// end of factory.h
