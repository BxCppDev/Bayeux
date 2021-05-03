/// \file geomtools/grid_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-21
 * Last modified: 2013-02-21
 *
 * License:
 *
 * Description:
 *   Grid model
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_GRID_MODEL_H
#define GEOMTOOLS_GRID_MODEL_H 1

// Standard library:
#include <string>
#include <iostream>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/regular_grid_placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/box.h>
#include <geomtools/i_stackable.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  class box;

  /// \brief A geometry model implementing a mother box volume with some daughter volumes positioned on a 2D grid
  class grid_model : public i_boxed_model
  {

  public:

    /// Return the number of replicated items along one of the grid axis
    size_t get_number_of_items (int) const;

    /// Set the replicated model
    void set_model (const i_model &);

    /// Return  a reference to the replicated model
    const i_model & get_model () const;

    /// Return the replication step along one of the grid axis
    double get_step (int) const;

    /// Return the X dimension of the mother box
    double get_x () const;

    /// Return the Y dimension of the mother box
    double get_y () const;

    /// Return the Z dimension of the mother box
    double get_z () const;

    /// Return a reference to the mother box
    const geomtools::box & get_box () const override;

    /// Return a reference to the mother box
    const geomtools::box & get_solid () const;

    /// Return the label used for daughter volumes set on the grid
    const std::string & get_grid_daughter_label () const;

    /// Constructor
    grid_model ();

    /// Destructor
    ~grid_model () override;

    /// Return the model unique class Id
    std::string get_model_id () const override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

  protected:

    /// Pre-construction
    void _pre_construct (datatools::properties & setup_,
                                 models_col_type * models_) override;

    /// Post construction
    void _post_construct (datatools::properties & setup_,
                          models_col_type * models_) override;

    /// Construction
    void _at_construct (const datatools::properties & config_,
                        models_col_type * models_) override;
  private:

    const i_model *            _model_ = nullptr; /// Replicated model
    regular_grid_placement     _grid_placement_; /// Replication placement on the grid
    physical_volume            _phys_; /// Physical volume
    std::string    _grid_daughter_label_; /// Labels for daughter volumes
    std::string    _grid_plane_label_; /// Label of the grid plane
    size_t         _number_of_items_[2]; /// Numbers of replication axis
    double         _x_; /// X dimension of the mother box
    double         _y_; /// Y dimension of the mother box
    double         _z_; /// Z dimension of the mother box
    double         _step_[2]; /// Replication step lengths
    stackable_data _sd_; /// Not used ?
    geomtools::box _solid_; /// Mother box
    MWIM           _internals_; /// internal items within the shape

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(grid_model)

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::grid_model)

#endif // GEOMTOOLS_GRID_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
