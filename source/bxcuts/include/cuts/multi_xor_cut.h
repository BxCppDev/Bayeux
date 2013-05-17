// -*- mode: c++ ; -*-
/* multi_xor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   XOR cut that combines many cuts
 *
 * History:
 *
 */

#ifndef CUTS_MULTI_XOR_CUT_H_
#define CUTS_MULTI_XOR_CUT_H_ 1

#include <string>
#include <list>

#include <cuts/i_multi_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {

  /// \brief A multi XOR cut
  class multi_xor_cut : public i_multi_cut
  {
  public:

    /// Cut interface macro
    CUT_INTERFACE_NOINIT_NORESET_CTOR_DTOR(multi_xor_cut);

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(multi_xor_cut);

  };

} // end of namespace cuts

#endif // CUTS_MULTI_XOR_CUT_H_

// end of multi_xor_cut.h
