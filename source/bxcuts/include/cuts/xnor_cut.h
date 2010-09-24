// -*- mode: c++ ; -*- 
/* xnor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   XNOR binary cut
 * 
 * History: 
 * 
 */

#ifndef __cuts__xnor_cut_h
#define __cuts__xnor_cut_h 1


#include <cuts/i_binary_cut.h>

namespace cuts {

  class xnor_cut : public i_binary_cut
  {
  
  public: 

    // ctor:
    xnor_cut ();

    // ctor:
    xnor_cut (i_cut &, i_cut &);

    // dtor:
    virtual ~xnor_cut ();

  protected:

    virtual bool _accept (); 

  private:

    static creator_registration<xnor_cut> __CR;

  public:

    virtual string cut_id () const;

    virtual cut_creator_t cut_creator () const;

    static i_cut * create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_ = 0);

  };

} // end of namespace cuts

#endif // __cuts__xnor_cut_h

// end of xnor_cut.h
