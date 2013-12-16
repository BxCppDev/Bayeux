// -*- mode: c++ ; -*-
/* nand_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-05-16
 *
 * License:
 *
 * Description:
 *   NAND binary cut
 *
 * History:
 *
 */

#ifndef CUTS_NAND_CUT_H_
#define CUTS_NAND_CUT_H_ 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The NAND binary cut
  class nand_cut : public i_binary_cut
  {

  public:

    /// Constructor
    nand_cut(datatools::logger::priority a_logging_priority =
             datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~nand_cut();

  protected :

    /// Selection
    virtual int _accept();

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(nand_cut);

  };

} // end of namespace cuts

#endif // CUTS_NAND_CUT_H_

// end of nand_cut.h
