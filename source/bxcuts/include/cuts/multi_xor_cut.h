/// \file cuts/multi_xor_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef CUTS_MULTI_XOR_CUT_H
#define CUTS_MULTI_XOR_CUT_H 1

// Standard library:
#include <string>
#include <list>

// This project:
#include <cuts/i_multi_cut.h>

namespace cuts {

  /// \brief A multi XOR cut
  class multi_xor_cut : public i_multi_cut
  {
  public:

    /// Constructor
    multi_xor_cut(datatools::logger::priority a_logging_priority =
                 datatools::logger::PRIO_FATAL);

    /// Destructor
    ~multi_xor_cut() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(multi_xor_cut)

  };

} // end of namespace cuts

#endif // CUTS_MULTI_XOR_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
