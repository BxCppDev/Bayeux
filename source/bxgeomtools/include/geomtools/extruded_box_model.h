/// \file geomtools/extruded_box_model.h
/* Author(s) :    Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2015-10-27
 * Last modified: 2021-04-23
 * Description:
 *
 *   Geometry model for an extruded box
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
#include <geomtools/i_wires_drawer.h>

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
    ~extruded_box_model() override;

    /// Return the string Id of the model
    std::string get_model_id() const override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_          = false) const override;

    /// Return the embedded extruded box
    const geomtools::extruded_box & get_extruded_box() const;

    /// \brief Special 3D rendering
    struct wires_drawer : public i_wires_drawer<extruded_box>
    {

      //! Constructor
      wires_drawer(const extruded_box & eb_);

      //! Destructor
      ~wires_drawer() override;

      //! Generate a list of polylines representing the contour of the shape (for display clients)
      void generate_wires_self(wires_type & wires_,
                               uint32_t options_ = 0) const override;
    };

  protected:

    /// Executed at construct
    void _at_construct(const datatools::properties & config_,
                       models_col_type * models_ = nullptr) override;

  private:

    std::string               _material_name_;   //!< Name of the material
    geomtools::extruded_box   _extruded_box_;    //!< Extruded box
    geomtools::box            _mother_box_;      //!< Reference box
    geomtools::box            _daughter_box_;    //!< Daughter box
    geomtools::subtraction_3d _subtraction_box_; //!< Top level extruded solid

    boost::scoped_ptr<wires_drawer> _drawer_;

    // Registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(extruded_box_model)

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::extruded_box_model)

#endif // GEOMTOOLS_EXTRUDED_BOX_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
