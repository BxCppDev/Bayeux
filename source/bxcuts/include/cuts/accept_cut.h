// -*- mode: c++ ; -*- 
/* accept_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   Cut that always accepts
 * 
 * History: 
 * 
 */

#ifndef __cuts__accept_cut_h
#define __cuts__accept_cut_h 1

#include <cstdlib>

#include <cuts/i_cut.h>

namespace cuts {

  class accept_cut : public i_cut
  {
    
  public: 
    // ctor:
    accept_cut ();

    // dtor:
    virtual ~accept_cut ();
  
    virtual void set_user_data (void *);

  protected:

    virtual bool _accept (); 

    /*************************************/

  private:

    static creator_registration<accept_cut> __CR;

  public:

    virtual string cut_id () const;

    virtual cut_creator_t cut_creator () const;

    static i_cut * create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_ = 0);

  };

} // end of namespace cuts

#endif // __cuts__accept_cut_h

// end of accept_cut.h
