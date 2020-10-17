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
    std::string get_model_id () const override;

    /// Default constructor
    plate_with_hole_model ();

    /// Destructor
    ~plate_with_hole_model () override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    /// \brief Special wires 3D rendering
    struct wires_drawer : public i_wires_drawer<plate_with_hole_model>
    {
      //! \brief Rendering options
      enum wires_rendering_option_type {
        WR_PWHM_NO_PLATE_FACES = (WR_BASE_LAST << 1),      //!< Do not render the mother plate solid faces
        WR_PWHM_NO_HOLE_FACES  = (WR_BASE_LAST << 2),      //!< Do not render the hole solid faces
        WR_PWHM_LAST           = (WR_PWHM_NO_HOLE_FACES),  //!< Last defined bit
        WR_PWHM_MASK           = (WR_PWHM_NO_PLATE_FACES
                                  | WR_PWHM_NO_HOLE_FACES) //!< Rendering options bit mask
      };

      //! Constructor
      wires_drawer(const plate_with_hole_model & model_);

      //! Destructor
      ~wires_drawer() override;

      //! Generate a list of polylines representing the contour of the shape (for display clients)
      void generate_wires_self(wires_type & wires_,
                                       uint32_t options_ = 0) const override;

    };

  protected:

    /// Construction
    void _at_construct(const std::string & name_,
                               const datatools::properties & setup_,
                               geomtools::models_col_type * models_ = 0) override;

  private:

    std::string               _material_; //!< material reference name
    geomtools::box            _mother_;   //!< Mother box to be extruded
    geomtools::box            _box_hole_; //!< Extrusion box volume
    geomtools::cylinder       _cyl_hole_; //!< Extrusion cylinder volume
    geomtools::subtraction_3d _solid_;    //!< Top level solid shape
    double                    _x_; //!< X box
    double                    _y_; //!< Y box
    double                    _z_; //!< Z box

    double                    _r_hole_; //!< Radius of the extrusion (cylinder case)
    double                    _x_hole_; //!< Length of the extrusion (box case)
    double                    _y_hole_; //!< Width of the extrusion (box case)
    double                    _z_hole_; //!< Depth of the extrusion
    double                    _x_pos_hole_; //!< X position of the extrusion w.r.t. to the mother box X-Y face
    double                    _y_pos_hole_; //!< Y position of the extrusion w.r.t. to the mother box X-Y face

    boost::scoped_ptr<wires_drawer> _drawer_; //!< Special wires 3D renderer

  private:

    // Registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE (plate_with_hole_model)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_PLATE_WITH_HOLE_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
