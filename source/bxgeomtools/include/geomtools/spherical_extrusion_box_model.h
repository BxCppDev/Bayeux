// -*- mode: c++ ; -*-
/// \file geomtools/spherical_extrusion_box_model.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-04
 * Last modified: 2015-02-08
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

#ifndef GEOMTOOLS_SPHERICAL_EXTRUSION_BOX_MODEL_H
#define GEOMTOOLS_SPHERICAL_EXTRUSION_BOX_MODEL_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_wires_drawer.h>

namespace geomtools {

  /// \brief A box volume with a spherical extrusion
  class spherical_extrusion_box_model : public i_model
  {
  public:

    const std::string & get_material() const;

    const geomtools::subtraction_3d & get_solid() const;

    virtual std::string get_model_id() const;

    spherical_extrusion_box_model();

    virtual ~spherical_extrusion_box_model();

    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const;

    /// \brief Special Gnuplot rendering
    struct wires_drawer : public i_wires_drawer
    {
      wires_drawer(const spherical_extrusion_box_model & model_, double h_, bool bottom_);
      virtual ~wires_drawer();
      virtual void generate_wires(std::ostream & out_,
                                  const geomtools::vector_3d & position_,
                                  const geomtools::rotation_3d & rotation_);
    private:
      const spherical_extrusion_box_model * _model_;
      double _h_;
      bool _bottom_;
    };

  protected:

    virtual void _at_construct(const std::string & name_,
                               const datatools::properties & setup_,
                               geomtools::models_col_type * models_ = 0);
  private:

    std::string               _material_;
    geomtools::sphere         _extrusion_;
    geomtools::box            _mother_;
    geomtools::subtraction_3d _solid_;
    double                    _x_;
    double                    _y_;
    double                    _z_;
    double                    _r_extrusion_;
    double                    _r_sphere_;
    double                    _h_;
    bool                      _bottom_;

    boost::scoped_ptr<wires_drawer> _drawer_;

    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(spherical_extrusion_box_model);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SPHERICAL_EXTRUSION_BOX_MODEL_H
