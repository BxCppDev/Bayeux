// -*- mode: c++ ; -*-
/// \file geomtools/extruded_box_model.h
/* Author(s) :    Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2015-10-27
 * Last modified: 2015-10-27
 *
 * License:
 *
 * Description:
 *
 *   Geometry model for an extruded box
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_EXTRUDED_BOX_MODEL_H
#define GEOMTOOLS_EXTRUDED_BOX_MODEL_H 1

// Standard libraries:
#include <iostream>
#include <string>
#include <exception>
#include <limits>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/model_macros.h>
#include <geomtools/box.h>
#include <geomtools/extruded_box.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/logical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  /// \brief A geometry model implementing a box with a cylindrical extrusion
  class extruded_box_model : public i_model
  {
  public:

    /// Set the name of the material
    void set_material_name(const std::string &);

    /// Return the name of the material
    const std::string & get_material_name() const;

    /// Return the shape of the extruded box
    const geomtools::i_shape_3d & get_solid() const;

    /// Default constructor
    extruded_box_model();

    /// Destructor
    virtual ~extruded_box_model();

    /// Return the string Id of the model
    virtual std::string get_model_id() const;

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const;

    /// Gnuplot draw function
    static void gnuplot_draw_user_function(std::ostream &,
                                           const geomtools::vector_3d &,
                                           const geomtools::rotation_3d &,
                                           const geomtools::i_object_3d &,
                                           void * = 0);

    /// Return the embedded extruded box
    const geomtools::extruded_box & get_extruded_box() const;

  protected:

    /// Executed at construct
    virtual void _at_construct(const std::string & name_,
                               const datatools::properties & config_,
                               models_col_type * models_ = 0);

  private:

    std::string               _material_name_;  //!< Name of the material
    geomtools::extruded_box   _extruded_box_;   //!< Extruded box

    geomtools::box            _mother_box_;      //!< Mother box
    geomtools::box            _daughter_box_;    //!< Daughter box
    geomtools::subtraction_3d _subtraction_box_; //!< Extruded box

    // Registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(extruded_box_model);

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::extruded_box_model)

#endif // GEOMTOOLS_EXTRUDED_BOX_MODEL_H
