// -*- mode: c++ ; -*-
/* and_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-05-16
 *
 * License:
 *
 * Description:
 *   AND binary cut
 *
 * History:
 *
 */

#ifndef CUTS_AND_CUT_H_
#define CUTS_AND_CUT_H_ 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The AND binary cut
  class and_cut : public cuts::i_binary_cut
  {
  public:

    /// Constructor
    and_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~and_cut();

  protected :

    /// Selection
    virtual int _accept();

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(and_cut);

  };

} // end of namespace cuts

#endif // CUTS_AND_CUT_H_

// end of and_cut.h
