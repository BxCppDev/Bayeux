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

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {

  CUT_CLASS_DECLARE (accept_cut)
  {
    
  public: 

    // ctor:
    CUT_INTERFACE_CTOR_DTOR(accept_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(accept_cut);

  };

} // end of namespace cuts

#endif // __cuts__accept_cut_h

// end of accept_cut.h
