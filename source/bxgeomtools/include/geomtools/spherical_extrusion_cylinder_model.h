// -*- mode: c++ ; -*-
/// \file geomtools/spherical_extrusion_cylinder_model.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-04
 * Last modified: 2012-04-04
 *
 * License:
 *
 * Description:
 *
 *   A cylinder extruded by a spherical volumes on top or bottom face.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SPHERICAL_EXTRUSION_CYLINDER_MODEL_H
#define GEOMTOOLS_SPHERICAL_EXTRUSION_CYLINDER_MODEL_H 1

// Standard library:
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/cylinder.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  /// \brief A cylinder volume with a spherical extrusion
  class spherical_extrusion_cylinder_model : public i_model
  {

  public:

    const std::string & get_material () const;

    const geomtools::subtraction_3d & get_solid () const;

    virtual std::string get_model_id () const;

    // ctor:
    spherical_extrusion_cylinder_model ();

    // dtor:
    virtual ~spherical_extrusion_cylinder_model ();

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    static void gnuplot_draw_user_function (std::ostream &,
                                            const geomtools::vector_3d &,
                                            const geomtools::rotation_3d &,
                                            const geomtools::i_object_3d &,
                                            void * = 0);
  protected:

    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & setup_,
                                geomtools::models_col_type * models_ = 0);
  private:

    std::string               _material_;
    geomtools::sphere         _extrusion_;
    geomtools::cylinder       _mother_;
    geomtools::subtraction_3d _solid_;
    double                    _r_;
    double                    _z_;
    double                    _r_extrusion_;
    double                    _r_sphere_;
    bool                      _bottom_;

    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE (spherical_extrusion_cylinder_model);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SPHERICAL_EXTRUSION_CYLINDER_MODEL_H
