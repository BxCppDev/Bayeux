// -*- mode: c++ ; -*- 
/* multi_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   A abstract cut interface that combines two other cuts
 * 
 * History: 
 * 
 */

#ifndef __cuts__multi_cut_h
#define __cuts__multi_cut_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <list>

#include <cuts/i_cut.h>

namespace cuts {
  
  using namespace std;

  class multi_cut : public i_cut
  {
  public:

    typedef list<i_cut *> cuts_col_t;

  protected: 
    
    cuts_col_t __cuts;
    
  public: 
    
    void add_cut (i_cut &);
    
    virtual void set_user_data (void *);
    
  public: 

    // ctor:
    multi_cut ();
    
    // dtor:
    virtual ~multi_cut ();

  protected:

    bool _accept ();

  private:

    static creator_registration<multi_cut> __CR;

  public:

    virtual string cut_id () const;

    virtual cut_creator_t cut_creator () const;

    static i_cut * create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_ = 0);

  };

} // end of namespace cuts

#endif // __cuts__multi_cut_h

// end of multi_cut.h
