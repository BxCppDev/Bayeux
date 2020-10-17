/// \file cuts/exclude_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-06-10
 * Last modified: 2015-06-10
 *
 * License:
 *
 * Description:
 *   EXCLUDE binary cut
 *
 * History:
 *
 */

#ifndef CUTS_EXCLUDE_CUT_H
#define CUTS_EXCLUDE_CUT_H 1

// This project:
#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The EXCLUDE binary cut (a OR NOT b)
  class exclude_cut : public i_binary_cut
  {
  public:

    /// Constructor
    exclude_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

    /// Destructor
    ~exclude_cut() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(exclude_cut)

  };

} // end of namespace cuts

#endif // CUTS_EXCLUDE_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
