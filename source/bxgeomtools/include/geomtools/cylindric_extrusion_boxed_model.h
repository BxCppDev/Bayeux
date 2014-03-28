// -*- mode: c++ ; -*-
/* cylindric_extrusion_boxed_model.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2014-03-27
 *
 * License:
 *
 * Description:
 *
 *   Geometry model for a box with a central cylindrical extrusion.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_CYLINDRIC_EXTRUSION_BOXED_MODEL_H_
#define GEOMTOOLS_CYLINDRIC_EXTRUSION_BOXED_MODEL_H_ 1

// Standard libraries:
#include <iostream>
#include <string>
#include <exception>
#include <limits>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/model_macros.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/logical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  // define a geometry model:
  GEOMTOOLS_MODEL_CLASS_DECLARE (cylindric_extrusion_boxed_model)
  {
  public:

    /// Set the name of the material
    void set_material_name (const std::string &);

    /// Return the name of the material
    const std::string & get_material_name () const;

    /// Set the X dimension of the mother box
    void set_mother_x (double);

    /// Set the Y dimension of the mother box
    void set_mother_y (double);

    /// Set the Z dimension of the mother box
    void set_mother_z (double);

    /// Set the radius of the cylindric extrusion
    void set_extrusion_radius (double);

    /// Set the X position of the cylindric extrusion's axis
    void set_extrusion_x_position (double);

    /// Set the Y position of the cylindric extrusion's axis
    void set_extrusion_y_position (double);

    /// Set the X and Y positions and radius of the cylindric extrusion
    void set_extrusion (double x_, double y_, double r_);

    /// Return the X dimension of the mother box
    double get_mother_x () const;

    /// Return the Y dimension of the mother box
    double get_mother_y () const;

    /// Return the Z dimension of the mother box
    double get_mother_z () const;

    /// Return the extrusion radius
    double get_extrusion_radius () const;

    /// Return the X position of the cylindric extrusion's axis
    double get_extrusion_x_position () const;

    /// Return the Y position of the cylindric extrusion's axis
    double get_extrusion_y_position () const;

    /// Return the shape of the mother box
    const geomtools::i_shape_3d & get_solid () const;

    /// Default constructor
    cylindric_extrusion_boxed_model ();

    /// Destructor
    virtual ~cylindric_extrusion_boxed_model ();

    /// Return the string Id of the model
    virtual std::string get_model_id () const;

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    /// Gnuplot draw function
    static void gnuplot_draw_user_function (std::ostream &,
                                            const geomtools::vector_3d &,
                                            const geomtools::rotation_3d &,
                                            const geomtools::i_object_3d &,
                                            void * = 0);

  protected:

    /// Executed at construct
    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);

  private:

    std::string      _material_name_; /// Name of the material
    double           _mother_x_, _mother_y_, _mother_z_; /// Dimensions of the mother box
    double           _extrusion_radius_; /// Radius of the cylindric extrusion
    double           _extrusion_x_; /// X position of the extrusion
    double           _extrusion_y_; /// Y position of the extrusion

    geomtools::box            _mother_box_;         /// Mother box
    geomtools::cylinder       _extrusion_cylinder_; /// Extrusion cylinder
    geomtools::subtraction_3d _extruded_solid_;     /// Extruded solid

    // Registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(cylindric_extrusion_boxed_model);

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::cylindric_extrusion_boxed_model)

#endif // GEOMTOOLS_CYLINDRIC_EXTRUSION_BOXED_MODEL_H_

// end of cylindric_extrusion_boxed_model.h
