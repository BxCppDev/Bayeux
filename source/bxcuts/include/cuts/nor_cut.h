// -*- mode: c++ ; -*-
/* nor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef CUTS_NOR_CUT_H_
#define CUTS_NOR_CUT_H_ 1

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
    virtual ~nor_cut();

  protected :

    /// Selection
    virtual int _accept();

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(nor_cut);

  };

} // end of namespace cuts

#endif // CUTS_NOR_CUT_H_

// end of nor_cut.h
