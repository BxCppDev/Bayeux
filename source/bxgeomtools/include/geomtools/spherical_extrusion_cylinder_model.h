/// \file geomtools/spherical_extrusion_cylinder_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-04
 * Last modified: 2015-02-15
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

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/cylinder.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_wires_drawer.h>

namespace geomtools {

  /// \brief A cylinder volume with a spherical extrusion
  class spherical_extrusion_cylinder_model : public i_model
  {

  public:

    //! Return the material label
    const std::string & get_material() const;

    //! Return the top level solid
    const geomtools::subtraction_3d & get_solid() const;

    //! Return the model unique identifier
    std::string get_model_id() const override;

    //! Constructor
    spherical_extrusion_cylinder_model();

    //! Destructor
    ~spherical_extrusion_cylinder_model() override;

    //! Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// \brief Special wires 3D rendering
    struct wires_drawer : public i_wires_drawer<spherical_extrusion_cylinder_model>
    {
      //! \brief Rendering options
      enum wires_rendering_option_type {
        WR_SECM_NO_MOTHER_FACES    = (WR_BASE_LAST << 1),           //!< Do not render the mother cylinder solid faces
        WR_SECM_NO_EXTRUSION_FACES = (WR_BASE_LAST << 2),           //!< Do not render the extrusion surface
        WR_SECM_LAST               = (WR_SECM_NO_MOTHER_FACES),     //!< Last defined bit
        WR_SECM_MASK               = (WR_SECM_NO_MOTHER_FACES
                                      | WR_SECM_NO_EXTRUSION_FACES) //!< Rendering options bit mask
      };

      //! Constructor
      wires_drawer(const spherical_extrusion_cylinder_model & model_);

      //! Destructor
      ~wires_drawer() override;

      //! Generate a list of polylines representing the contour of the shape (for display clients)
      void generate_wires_self(wires_type & wires_,
                                       uint32_t options_ = 0) const override;

    };

  protected:

    void _at_construct(const std::string & name_,
                               const datatools::properties & setup_,
                               geomtools::models_col_type * models_ = 0) override;
  private:

    std::string               _material_; //!< Material name
    geomtools::sphere         _extrusion_;
    geomtools::cylinder       _mother_;
    geomtools::subtraction_3d _solid_;
    double                    _r_;
    double                    _z_;
    double                    _r_extrusion_;
    double                    _r_sphere_;
    double                    _h_;
    bool                      _bottom_;

    boost::scoped_ptr<wires_drawer> _drawer_;

    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(spherical_extrusion_cylinder_model)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SPHERICAL_EXTRUSION_CYLINDER_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
