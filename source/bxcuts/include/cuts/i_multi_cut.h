// -*- mode: c++ ; -*-
/* i_multi_cut.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-16
 * Last modified: 2013-05-16
 *
 * License:
 *
 * Description:
 *   A abstract cut interface that combines many different cuts
 *
 * History:
 *
 */

#ifndef CUTS_I_MULTI_CUT_H_
#define CUTS_I_MULTI_CUT_H_ 1

#include <string>

#include <datatools/properties.h>

#include <cuts/i_cut.h>

namespace cuts {

  /// \brief The abstract base class for multi cuts
  class i_multi_cut : public cuts::i_cut
  {
  public:

    typedef std::list<cut_handle_type> cuts_col_type;

    void add_cut(cut_handle_type &);

    /// Constructor
    i_multi_cut(datatools::logger::priority a_logging_priority =
                 datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~i_multi_cut();

    /// Initialization
    virtual void initialize(const datatools::properties &,
                            datatools::service_manager &,
                            cuts::cut_handle_dict_type &);

    /// Reset
    virtual void reset();

  protected:

    void _install_cuts(const datatools::properties & a_configuration,
                       cuts::cut_handle_dict_type & a_cut_dict);

    virtual void _at_set_user_data();

    virtual void _at_reset_user_data();

    void _reset_cuts();

  protected:

    cuts_col_type _cuts;

  };

} // end of namespace cuts

#endif // CUTS_I_MULTI_CUT_H_

// end of i_multi_cut.h
