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

#ifndef _CUTS_XNOR_CUT_H
#define _CUTS_XNOR_CUT_H 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The XNOR binary cut
  class xnor_cut : public i_binary_cut
  {
  
  public: 

    CUT_INTERFACE_NORESET_CTOR_DTOR (xnor_cut);

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(xnor_cut);

  };

} // end of namespace cuts

#endif // _CUTS_XNOR_CUT_H

// end of xnor_cut.h
