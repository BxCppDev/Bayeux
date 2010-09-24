// -*- mode: c++ ; -*- 
/* factory.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-19
 * Last modified: 2010-09-19
 * 
 * License: 
 * 
 * Description: 
 *   Cut factory: 
 *   The factory enables the dynamic creation of an
 *   arbitrary cut from its ID (as registered in the
 *   cut_db static instance [see 'i_cut.h'])
 *   using some list of setup properties stored in some 'properties'
 *   object and some optional user data provided through a void * pointer
 *   in the 'create_cut' method.
 *
 * History: 
 * 
 */

#ifndef __cuts__factory_h
#define __cuts__factory_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <cuts/i_cut.h>

namespace cuts {

  using namespace std;

  class factory
  {
  public: 
    static bool g_devel;

  private: 
    bool               __debug;

  protected:
    cut_creator_dict_t _creators;

  public: 

    bool is_debug () const;

    void set_debug (bool);
  
  public: 

    // ctor:
    factory (bool preload_ = true);

    // dtor:
    virtual ~factory ();
 
    void do_register (const cut_creator_t & creator_, 
		      const string & cut_id_ = "");
    
    // from the i_cut:
    virtual i_cut * create_cut (const string & cut_id_,
				const datatools::utils::properties & cut_configuration_, 
				cut_dict_t & cut_dict_,
				void * user_ = 0);
    
    void dump_cuts (ostream & out_ = clog,
		    const string & title_ = "",
		    const string & indent_ = "") const;

    const cut_creator_dict_t & get_creators () const;   

    cut_creator_dict_t & get_creators ();   

  };

} // end of namespace cuts

#endif // __cuts__factory_h

// end of factory.h
