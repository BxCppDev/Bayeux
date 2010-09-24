// -*- mode: c++ ; -*- 
/* and_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   AND binary cut
 * 
 * History: 
 * 
 */

#ifndef __cuts__and_cut_h
#define __cuts__and_cut_h 1


#include <cuts/i_binary_cut.h>

namespace cuts {

  class and_cut : public i_binary_cut
  {
  
  public: 

    // ctor:
    and_cut ();

    // ctor:
    and_cut (i_cut &, i_cut &);

    // dtor:
    virtual ~and_cut ();

  protected:

    virtual bool _accept (); 

  private:

    static creator_registration<and_cut> __CR;

  public:

    virtual string cut_id () const;

    virtual cut_creator_t cut_creator () const;

    static i_cut * create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_ = 0);

  };

} // end of namespace cuts

#endif // __cuts__and_cut_h

// end of and_cut.h
