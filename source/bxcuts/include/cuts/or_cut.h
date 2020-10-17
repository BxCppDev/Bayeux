/// \file cuts/or_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-05-16
 *
 * License:
 *
 * Description:
 *   OR binary cut
 *
 * History:
 *
 */

#ifndef CUTS_OR_CUT_H
#define CUTS_OR_CUT_H 1

// This project:
#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The OR binary cut
  class or_cut : public i_binary_cut
  {
  public:

    /// Constructor
    or_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

    /// Destructor
    ~or_cut() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(or_cut)

  };

} // end of namespace cuts

#endif // CUTS_OR_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
