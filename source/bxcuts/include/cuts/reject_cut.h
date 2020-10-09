/// \file cuts/reject_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   Cut that always rejects
 *
 * History:
 *
 */

#ifndef CUTS_REJECT_CUT_H
#define CUTS_REJECT_CUT_H 1

// This project:
#include <cuts/i_cut.h>

namespace cuts {

  /// \brief A cut that always rejects
  class reject_cut : public i_cut
  {

  public:

    /// Constructor
    reject_cut(datatools::logger::priority a_logging_priority =
               datatools::logger::PRIO_FATAL);

    /// Destructor
    ~reject_cut() override;

    /// Initialization
    void initialize(const datatools::properties &,
                            datatools::service_manager &,
                            cuts::cut_handle_dict_type &) override;

    /// Reset
    void reset() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(reject_cut)

  };

} // end of namespace cuts

#endif // CUTS_REJECT_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
