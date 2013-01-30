// -*- mode: c++ ; -*- 
/* xor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   XOR binary cut
 * 
 * History: 
 * 
 */

#ifndef _CUTS_XOR_CUT_H
#define _CUTS_XOR_CUT_H 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The XOR binary cut
  class xor_cut : public i_binary_cut
  {
  
  public: 

    // ctor:
    CUT_INTERFACE_NORESET_CTOR_DTOR (xor_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(xor_cut);

  };

} // end of namespace cuts

#endif // _CUTS_XOR_CUT_H

// end of xor_cut.h
