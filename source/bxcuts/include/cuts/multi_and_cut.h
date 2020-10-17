/// \file cuts/multi_and_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-05-16
 *
 * License:
 *
 * Description:
 *   AND cut that combines many cuts
 *
 * History:
 *
 */

#ifndef CUTS_MULTI_AND_CUT_H
#define CUTS_MULTI_AND_CUT_H 1

// Standard library:
#include <string>
#include <list>

// This project:
#include <cuts/i_multi_cut.h>

namespace cuts {

  /// \brief A multi AND cut
  class multi_and_cut : public i_multi_cut
  {
  public:

    /// Constructor
    multi_and_cut(datatools::logger::priority a_logging_priority =
                  datatools::logger::PRIO_FATAL);

    /// Destructor
    ~multi_and_cut() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(multi_and_cut)

  };

} // end of namespace cuts

#endif // CUTS_MULTI_AND_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
