/// \file cuts/xnor_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef CUTS_XNOR_CUT_H
#define CUTS_XNOR_CUT_H 1

// This project:
#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The XNOR binary cut
  class xnor_cut : public i_binary_cut
  {
  public:

    /// Constructor
    xnor_cut(datatools::logger::priority a_logging_priority =
             datatools::logger::PRIO_FATAL);

    /// Destructor
    ~xnor_cut() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(xnor_cut)

  };

} // end of namespace cuts

#endif // CUTS_XNOR_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
