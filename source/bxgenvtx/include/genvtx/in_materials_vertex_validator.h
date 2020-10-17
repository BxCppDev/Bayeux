/// \file genvtx/in_materials_vertex_validator.h
/* Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-07-16
 * Last modified: 2014-07-16
 *
 * License: GPL 3.0
 *
 * Description:
 *
 * History:
 *
 */

#ifndef GENVTX_IN_MATERIALS_VERTEX_VALIDATOR_H
#define GENVTX_IN_MATERIALS_VERTEX_VALIDATOR_H 1

// Standard library:
#include <string>
#include <set>

// Third party:
// - Bayeux/cuts:
#include <cuts/i_cut.h>

namespace genvtx {

  /// \brief The vertex validation
  class in_materials_vertex_validator : public cuts::i_cut
  {
  public:

    /// Constructor
    in_materials_vertex_validator(datatools::logger::priority a_logging_priority =
                                      datatools::logger::PRIO_FATAL);

    /// Destructor
    ~in_materials_vertex_validator() override;

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

    /// Add the name of a material to be validated
    void add_material(const std::string & material_name_);

    /// Check if a given material name is set
    bool has_material(const std::string & material_name_) const;

    /// Check the use of a maximum depth in the hierarchy of daughter volumes to investigate materials
    bool has_max_depth() const;

    /// Set the maximum depth in the hierarchy of daughter volumes to investigate materials
    void set_max_depth(int);

    /// Return the maximum depth in the hierarchy of daughter volumes to investigate materials
    int get_max_depth() const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  protected :

    /// Selection
    int _accept() override;

  private:

    bool _reversed_; //!< Reverse flag
    std::set<std::string> _materials_; //!< List of materials to be validated
    int _max_depth_; //!< Maximum depth in the hierarchy of daughter volumes to investigate materials

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(in_materials_vertex_validator)

  };

} // end of namespace genvtx

#endif // GENVTX_IN_MATERIALS_VERTEX_VALIDATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
