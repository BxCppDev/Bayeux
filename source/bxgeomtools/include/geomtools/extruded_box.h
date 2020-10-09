/// \file geomtools/extruded_box.h
/* Author(s):     Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 *                François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-10-27
 * Last modified: 2015-03-23
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_EXTRUDED_BOX_H
#define GEOMTOOLS_EXTRUDED_BOX_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/placement.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward class declaration:
  class box;
  class composite_surface;

  /// \brief The 3D shape model for an extruded box
  class extruded_box : public i_shape_3d,
                       public i_stackable
  {

  public:

    /// Return the extruded_box shape label
    static const std::string & extruded_box_label();

    ///  \brief Masks used for the 6 faces of the extruded_box
    enum faces_mask_type {
      FACE_NONE          = face_identifier::FACE_BITS_NONE,
      FACE_BACK          = datatools::bit_mask::bit00,
      FACE_FRONT         = datatools::bit_mask::bit01,
      FACE_LEFT          = datatools::bit_mask::bit02,
      FACE_RIGHT         = datatools::bit_mask::bit03,
      FACE_BOTTOM        = datatools::bit_mask::bit04,
      FACE_TOP           = datatools::bit_mask::bit05,
      FACE_INSIDE_BACK   = datatools::bit_mask::bit06,
      FACE_INSIDE_FRONT  = datatools::bit_mask::bit07,
      FACE_INSIDE_LEFT   = datatools::bit_mask::bit08,
      FACE_INSIDE_RIGHT  = datatools::bit_mask::bit09,
      FACE_INSIDE_BOTTOM = datatools::bit_mask::bit10,
      FACE_INSIDE_TOP    = datatools::bit_mask::bit11,
      FACE_ALL           = (FACE_BACK
                            | FACE_FRONT
                            | FACE_LEFT
                            | FACE_RIGHT
                            | FACE_BOTTOM
                            | FACE_TOP
                            | FACE_INSIDE_BACK
                            | FACE_INSIDE_FRONT
                            | FACE_INSIDE_LEFT
                            | FACE_INSIDE_RIGHT
                            | FACE_INSIDE_BOTTOM
                            | FACE_INSIDE_TOP)
    };

    /// Return the min X coordinates (bounding box)
    double get_xmin() const override;

    /// Return the max X coordinates (bounding box)
    double get_xmax() const override;

    /// Return the min Y coordinates (bounding box)
    double get_ymin() const override;

    /// Return the max Y coordinates (bounding box)
    double get_ymax() const override;

    /// Return the min Z coordinates (bounding box)
    double get_zmin() const override;

    /// Return the max Z coordinates (bounding box)
    double get_zmax() const override;

    /// Return the X dimension
    double get_x() const;

    /// Set the X dimension
    void set_x(double);

    /// Return the half the X dimension
    double get_half_x() const;

    /// Set the half the X dimension
    void set_half_x(double);

    /// Return the Y dimension
    double get_y() const;

    /// Set the Y dimension
    void set_y(double);

    /// Return the half the Y dimension
    double get_half_y() const;

    /// Set the half the Y dimension
    void set_half_y(double);

    /// Return the Z dimension
    double get_z() const;

    /// Set the Z dimension
    void set_z(double);

    /// Return the half the Z dimension
    double get_half_z() const;

    /// Set the half the Z dimension
    void set_half_z(double);

    /// Set all dimensions
    void set(double, double, double, double);

    /// Set the halves of all dimensions
    void set_half(double, double, double, double);

    /// Return the thickness of extruded box
    double get_thickness() const;

    /// Set the thickness of extruded box
    void set_thickness(double);

    /// Return if extruded box has a bottom
    bool has_bottom() const;

    /// Set a bottom to the extruded box
    void set_bottom(bool);

    /// Add a bottom to the extruded box
    void add_bottom();

    /// Remove the bottom of the extruded box
    void remove_bottom();

    /// Return if extruded box has a top
    bool has_top() const;

    /// Set a top to the extruded box
    void set_top(bool);

    /// Add a top to the extruded box
    void add_top();

    /// Remove the top of the extruded box
    void remove_top();

    /// Default constructor
    extruded_box();

    /// Constructor
    extruded_box(double x_, double y_, double z_, double t_,
                 bool has_top_ = true, bool has_bottom_ = true);

    /// Destructor
    ~extruded_box() override;

    /// Return the name of the shape
    std::string get_shape_name() const override;

    // Check the validity of the object
    bool is_valid() const override;

    /// Initialize the extruded_box from properties
    void initialize(const datatools::properties &,
                            const handle_dict_type *) override;

    /// Reset
    void reset() override;

    /// Return the value of some parameter given by name
    double get_parameter(const std::string &) const;

    /// Return the cumulated surface of the extruded_box given a list of faces
    double get_surface(uint32_t mask_ = FACE_ALL) const override;

    /// Return the volume of the extruded_box
    double get_volume(uint32_t flags_ = 0) const override;

    /// Check if a point is inside the extruded_box
    bool is_inside(const vector_3d &,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is inside the extruded_box
    bool is_outside(const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_bits_any(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the vector normal to the surface at some position
    vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const override;

    /// Print operator
    friend std::ostream & operator<<( std::ostream &, const extruded_box & );

    /// Read operator
    friend std::istream & operator>>( std::istream &, extruded_box & );

    /// Find the intercept point of a segment with the extruded_box
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// Return a collection of face info objects
    unsigned int compute_faces(face_info_collection_type & faces_) const override;

    /// Compute the outer box
    void compute_outer_box(box & outer_box_) const;

    /// Compute the inner box and its placement
    void compute_inner_box(box & inner_box_, placement & face_placement_) const;

    /// Compute the top face and placement
    void compute_extruded_top_bottom_face(faces_mask_type face_id_,
                                          composite_surface & face_,
                                          placement & face_placement_) const;

    /// \brief 3D rendering options
    enum extruded_box_wires_rendering_option_type {
      WR_EXTRBOX_NO_EXTERNAL_FACES = (WR_BASE_LAST << 1),             //!< Do not render the external faces
      WR_EXTRBOX_NO_INTERNAL_FACES = (WR_BASE_LAST << 2),             //!< Do not render the internal faces
      WR_EXTRBOX_LAST              = (WR_EXTRBOX_NO_INTERNAL_FACES),  //!< Last defined bit
      WR_EXTRBOX_MASK              = (WR_EXTRBOX_NO_EXTERNAL_FACES
                                      | WR_EXTRBOX_NO_INTERNAL_FACES) //!< Rendering options bit mask
    };

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_default();

    /// Build the bounding data
    void _build_bounding_data() override;

  private:

    double _x_;          //!< Width (in arbitrary units)
    double _y_;          //!< Length (in arbitrary units)
    double _z_;          //!< Height (in arbitrary units)
    double _thickness_;  //!< Thickness (in arbitrary units)
    bool   _has_top_;    //!< Flag if extruded box has a top
    bool   _has_bottom_; //!< Flag if extruded box has a bottom

    // // Work
    // datatools::handle<box> _h_outer_box_;
    // datatools::handle<box> _h_inner_box_;
    // placement              _inner_box_placement_;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(extruded_box)

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::extruded_box the name the class with registered OCD support
DOCD_CLASS_DECLARATION(geomtools::extruded_box)

#endif // GEOMTOOLS_EXTRUDED_BOX_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
