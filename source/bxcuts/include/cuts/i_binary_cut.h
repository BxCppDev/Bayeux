/// \file cuts/i_binary_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-05-16
 *
 * License:
 *
 * Description:
 *   A abstract cut interface that combines two other cuts
 *
 * History:
 *
 */

#ifndef CUTS_I_BINARY_CUT_H
#define CUTS_I_BINARY_CUT_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <cuts/i_cut.h>

namespace cuts {

  /// \brief The abstract base class for binary cuts
  class i_binary_cut : public cuts::i_cut
  {
  public:

    void set_cut_1(cut_handle_type &);

    void set_cut_2(cut_handle_type &);

    void set_cuts(cut_handle_type &, cut_handle_type &);

    /// Constructor
    i_binary_cut(datatools::logger::priority a_logging_priority =
                 datatools::logger::PRIO_FATAL);

    /// Destructor
    ~i_binary_cut() override;

    /// Initialization
    void initialize(const datatools::properties &,
                            datatools::service_manager &,
                            cuts::cut_handle_dict_type &) override;

    /// Export to a container of properties
    void export_to_config(datatools::properties & config_,
                                  uint32_t flags_ = i_cut::EXPORT_CONFIG_DEFAULT,
                                  const std::string & prefix_ = "") const override;

    /// Reset
    void reset() override;

    /// Smart print
    void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit               = false) const override;

  protected:

    void _install_cuts(const datatools::properties & a_configuration,
                       cuts::cut_handle_dict_type & a_cut_dict);

    void _at_set_user_data() override;

    void _at_reset_user_data() override;

    void _reset_cuts();

  protected:

    cut_handle_type _handle_1;
    cut_handle_type _handle_2;

  };

} // end of namespace cuts

#endif // CUTS_I_BINARY_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
