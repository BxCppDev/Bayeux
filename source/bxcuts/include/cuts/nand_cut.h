// -*- mode: c++ ; -*- 
/* nand_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   NAND binary cut
 * 
 * History: 
 * 
 */

#ifndef __cuts__nand_cut_h
#define __cuts__nand_cut_h 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  class nand_cut : public i_binary_cut
  {
  
  public: 

    // ctor:
    CUT_INTERFACE_NORESET_CTOR_DTOR (nand_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(nand_cut);

  };

} // end of namespace cuts

#endif // __cuts__nand_cut_h

// end of nand_cut.h
