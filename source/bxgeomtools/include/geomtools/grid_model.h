// -*- mode: c++ ; -*-
/* grid_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_GRID_MODEL_H_
#define GEOMTOOLS_GRID_MODEL_H_ 1

#include <string>
#include <iostream>

#include <geomtools/i_boxed_model.h>
#include <geomtools/regular_grid_placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/box.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  class box;

  // define a geometry model with a single box:
  GEOMTOOLS_BOXED_MODEL_CLASS_DECLARE(grid_model)
  {

  public:

    size_t get_number_of_items (int);

    void set_model (const i_model &);

    const i_model & get_model () const;

    double get_step (int) const;

    double get_x () const;

    double get_y () const;

    double get_z () const;

    virtual const geomtools::box & get_box () const;

    const geomtools::box & get_solid () const;

    const std::string & get_grid_daughter_label () const;

    grid_model ();

    virtual ~grid_model ();

    virtual std::string get_model_id () const;

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  protected:

    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);
  private:

    const i_model *            _model_;
    regular_grid_placement     _grid_placement_;
    physical_volume            _phys_;
    std::string    _grid_daughter_label_;
    std::string    _grid_plane_label_;
    size_t         _number_of_items_[2];
    double         _x_;
    double         _y_;
    double         _z_;
    double         _step_[2];
    stackable_data _sd_;
    geomtools::box _solid_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(grid_model);

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::grid_model)

#endif // GEOMTOOLS_GRID_MODEL_H_

// end of grid_model.h
