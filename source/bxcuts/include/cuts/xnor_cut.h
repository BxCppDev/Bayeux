// -*- mode: c++ ; -*-
/* xnor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-05-15
 *
 * License:
 *
 * Description:
 *   XNOR binary cut
 *
 * History:
 *
 */

#ifndef CUTS_XNOR_CUT_H_
#define CUTS_XNOR_CUT_H_ 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The XNOR binary cut
  class xnor_cut : public i_binary_cut
  {
  public:

    /// Cut interface macro
    CUT_INTERFACE_NOINIT_NORESET_CTOR_DTOR (xnor_cut);

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(xnor_cut);

  };

} // end of namespace cuts

#endif // CUTS_XNOR_CUT_H_

// end of xnor_cut.h
