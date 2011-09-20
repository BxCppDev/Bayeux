// -*- mode: c++ ; -*- 
/* or_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   OR binary cut
 * 
 * History: 
 * 
 */

#ifndef __cuts__or_cut_h
#define __cuts__or_cut_h 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  class or_cut : public i_binary_cut
  {
  
  public: 

    // ctor:
    CUT_INTERFACE_NORESET_CTOR_DTOR (or_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(or_cut);

  };

} // end of namespace cuts

#endif // __cuts__or_cut_h

// end of or_cut.h
