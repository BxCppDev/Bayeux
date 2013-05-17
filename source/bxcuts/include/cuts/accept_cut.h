// -*- mode: c++ ; -*-
/* accept_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   Cut that always accepts
 *
 * History:
 *
 */

#ifndef CUTS_ACCEPT_CUT_H_
#define CUTS_ACCEPT_CUT_H_ 1

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {

  /// \brief A cut that always accepts
  CUT_CLASS_DECLARE (accept_cut)
  {
  public:
    /// Constructor
    CUT_INTERFACE_CTOR_DTOR(accept_cut);

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(accept_cut);

  };

} // end of namespace cuts

#endif // CUTS_ACCEPT_CUT_H_

// end of accept_cut.h
