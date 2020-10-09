/// \file genvtx/not_in_daughters_vertex_validator.h
/* Author(s) : Xavier Garrido <garrido@lal.in2p3.fr>
 *             Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-07-06
 * Last modified: 2014-07-14
 *
 * License: GPL 3.0
 *
 * Description:
 *
 * History:
 *
 */

#ifndef GENVTX_NOT_IN_DAUGHTERS_VERTEX_VALIDATOR_H
#define GENVTX_NOT_IN_DAUGHTERS_VERTEX_VALIDATOR_H 1

// Third party:
// - Bayeux/cuts:
#include <cuts/i_cut.h>

namespace genvtx {

  /// \brief Vertex validator that select only vertex shoot from a given volume but not in its daughter volumes
  class not_in_daughters_vertex_validator : public cuts::i_cut
  {
  public:

    /// Constructor
    not_in_daughters_vertex_validator(datatools::logger::priority a_logging_priority =
                                      datatools::logger::PRIO_FATAL);

    /// Destructor
    ~not_in_daughters_vertex_validator() override;

    /// Initialization
    void initialize(const datatools::properties &,
                            datatools::service_manager &,
                            cuts::cut_handle_dict_type &) override;

    /// Reset
    void reset() override;

    /// Check reversed flag
    bool is_reversed() const;

    /// Set the reversed flag
    void set_reversed(bool);

    /// Set the tolerance to check mother volume
    void set_tolerance(double);

    /// Set the tolerance to check daughter volume
    void set_daughter_tolerance(double);

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  protected :

    /// Main selection of the vertex
    /**
     *  The user data must be of type 'genvtx::vertex_validation::geometry_context';
     *  it must contain :
     *  - a reference to a logical volume from where the candidate vertex has been shot
     *  - a reference to the candidate vertex expressed in the local reference frame of the logical volume
     */
    int _accept() override;

  private:

    bool   _reversed_;           //!< Reversed flag
    double _tolerance_;          //!< Tolerance to check mother volume
    double _daughter_tolerance_; //!< Tolerance to check daughter volumes

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(not_in_daughters_vertex_validator)

  };

} // end of namespace genvtx

#endif // GENVTX_NOT_IN_DAUGHTERS_VERTEX_VALIDATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
