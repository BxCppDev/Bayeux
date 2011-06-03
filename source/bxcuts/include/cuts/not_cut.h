// -*- mode: c++ ; -*- 
/* not_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   NOT logic cut.
 * 
 * History: 
 * 
 */

#ifndef __cuts__not_cut_h
#define __cuts__not_cut_h 1

#include <cuts/i_cut.h>

namespace cuts {
  
  class not_cut : public i_cut
  {
  protected: 
    
    i_cut * _cut;
    
  public: 
    
    void set_cut (i_cut &);
    
    virtual void set_user_data (void *);
    
  public: 

    // ctor:
    not_cut ();
    
    // ctor:
    not_cut (i_cut &);
    
    // dtor:
    virtual ~not_cut ();

  protected:

    virtual bool _accept (); 

  private:

    static creator_registration<not_cut> __CR;

  public:

    virtual string cut_id () const;

    virtual cut_creator_t cut_creator () const;

    static i_cut * create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_ = 0);
  
  };

} // end of namespace cuts

#endif // __cuts__not_cut_h

// end of not_cut.h
