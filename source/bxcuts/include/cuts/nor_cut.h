/// \file cuts/nor_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-05-16
 *
 * License:
 *
 * Description:
 *   NOR binary cut
 *
 * History:
 *
 */

#ifndef CUTS_NOR_CUT_H
#define CUTS_NOR_CUT_H 1

// This project:
#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The NOR binary cut
  class nor_cut : public i_binary_cut
  {

  public:

    /// Constructor
    nor_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

    /// Destructor
    ~nor_cut() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(nor_cut)

  };

} // end of namespace cuts

#endif // CUTS_NOR_CUT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
