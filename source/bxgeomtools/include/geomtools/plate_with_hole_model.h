// -*- mode: c++ ; -*-
/// \file geomtools/plate_with_hole_model.h
/* Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-04
 * Last modified: 2014-05-19
 *
 * License:
 *
 * Description:
 *
 *   A box extruded by a spherical volumes on top or bottom face.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_PLATE_WITH_HOLE_MODEL_H
#define GEOMTOOLS_PLATE_WITH_HOLE_MODEL_H 1

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_wires_drawer.h>

namespace geomtools {

  /// \brief A geometry model implementing a box/plate with cylindrical hole
  /// This class is similar to the geomtools::cylindric_extrusion_boxed_model
  /// class.
  class plate_with_hole_model : public i_model
  {
  public:

    /// Return the name of the material
    const std::string & get_material () const;

    /// Return the top level solid
    const geomtools::subtraction_3d & get_solid () const;

    /// Return the geometry model class Id
    virtual std::string get_model_id () const;

    /// Default constructor
    plate_with_hole_model ();

    /// Destructor
    virtual ~plate_with_hole_model ();

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    /// \brief  Special Gnuplot rendering
    struct wires_drawer : public i_wires_drawer
    {
      wires_drawer(const plate_with_hole_model & model_,
                   double x_pos_hole_, double y_pos_hole_);
      virtual ~wires_drawer();
      virtual void generate_wires(std::ostream & out_,
                                  const geomtools::vector_3d & position_,
                                  const geomtools::rotation_3d & rotation_);
    private:
      const plate_with_hole_model * _model_;
      double _x_pos_hole_;
      double _y_pos_hole_;
    };

  protected:

    /// Construction
    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & setup_,
                                geomtools::models_col_type * models_ = 0);

  private:

    std::string               _material_;
    geomtools::box            _mother_;
    geomtools::box            _box_hole_;
    geomtools::cylinder       _cyl_hole_;
    geomtools::subtraction_3d _solid_;
    double                    _x_; //!< X box
    double                    _y_; //!< Y box
    double                    _z_; //!< Z box

    double                    _r_hole_;
    double                    _x_hole_;
    double                    _y_hole_;
    double                    _z_hole_;
    double                    _x_pos_hole_;
    double                    _y_pos_hole_;

    boost::scoped_ptr<wires_drawer> _drawer_;

  private:

    // Registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE (plate_with_hole_model);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_PLATE_WITH_HOLE_MODEL_H
