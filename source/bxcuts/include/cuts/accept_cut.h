/// \file cuts/accept_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   Cut that always accepts
 *
 * History:
 *
 */

#ifndef CUTS_ACCEPT_CUT_H
#define CUTS_ACCEPT_CUT_H 1

// This project:
#include <cuts/i_cut.h>

namespace cuts {

  /// \brief A cut that always accepts
  class accept_cut : public cuts::i_cut
  {
  public:

    /// Constructor
    accept_cut(datatools::logger::priority a_logging_priority =
               datatools::logger::PRIO_FATAL);

    /// Destructor
    ~accept_cut() override;

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

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(accept_cut)

  };

} // end of namespace cuts

#endif // CUTS_ACCEPT_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
