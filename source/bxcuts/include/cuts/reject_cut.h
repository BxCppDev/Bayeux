// -*- mode: c++ ; -*- 
/* reject_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   Cut that always rejects
 * 
 * History: 
 * 
 */

#ifndef _CUTS_REJECT_CUT_H
#define _CUTS_REJECT_CUT_H 1

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {

  //CUT_CLASS_DECLARE (reject_cut)
  /// \brief A cut that always rejects
  class reject_cut : public i_cut
  {
    
  public: 

    // ctor:
    CUT_INTERFACE_CTOR_DTOR (reject_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(reject_cut);

  };

} // end of namespace cuts

#endif // _CUTS_REJECT_CUT_H

// end of reject_cut.h
