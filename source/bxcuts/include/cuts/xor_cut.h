/// \file cuts/xor_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   XOR binary cut
 *
 * History:
 *
 */

#ifndef CUTS_XOR_CUT_H
#define CUTS_XOR_CUT_H 1

// This project:
#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The XOR binary cut
  class xor_cut : public i_binary_cut
  {
  public:

    /// Constructor
    xor_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

    /// Destructor
    ~xor_cut() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(xor_cut)

  };

} // end of namespace cuts

#endif // CUTS_XOR_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
