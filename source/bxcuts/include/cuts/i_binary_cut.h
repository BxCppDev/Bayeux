// -*- mode: c++ ; -*- 
/* i_binary_cut.h
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

#ifndef __cuts__i_binary_cut_h
#define __cuts__i_binary_cut_h 1

#include <cuts/i_cut.h>

namespace cuts {
  
  class i_binary_cut : public i_cut
  {
  protected: 
    
    i_cut * _cut_1;
    i_cut * _cut_2;
    
  public: 
    
    void set_cut_1 (i_cut &);
    
    void set_cut_2 (i_cut &);
    
    void set_cuts (i_cut &, i_cut &);
    
    virtual void set_user_data (void *);
    
  public: 

    // ctor:
    i_binary_cut ();
    
    // ctor:
    i_binary_cut (i_cut &, i_cut &);
    
    // dtor:
    virtual ~i_binary_cut ();

    static void install_cuts (const properties & configuration_, 
			      cut_dict_t * cut_dict_, 
			      i_binary_cut & binary_cut_);
  
  };

} // end of namespace cuts

#endif // __cuts__i_binary_cut_h

// end of i_binary_cut.h
