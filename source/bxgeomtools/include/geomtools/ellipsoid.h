/// \file geomtools/ellipsoid.h
/* Author(s):     A. Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2014-11-18
 * Last modified: 2014-11-18
 *
 * License:
 *
 * Description:
 *   3D ellipsoid description
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_ELLIPSOID_H
#define GEOMTOOLS_ELLIPSOID_H 1

// Standard library:
#include <string>
#include <iostream>

// Third party
// - Bayeux/datatools:
#include <datatools/bit_mask.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward class declarations:
  class elliptical_sector;
  class ellipsoid_sector;

  /// \brief The 3D shape model for an ellipsoid
  class ellipsoid : public i_shape_3d,
                    public i_stackable
  {
  public:

    enum faces_mask_type {
      FACE_NONE   = face_identifier::FACE_BITS_NONE,
      FACE_SIDE   = datatools::bit_mask::bit00,
      FACE_TOP    = datatools::bit_mask::bit01,
      FACE_BOTTOM = datatools::bit_mask::bit02,
      FACE_ALL    = FACE_SIDE | FACE_TOP | FACE_BOTTOM
    };

    static const std::string & ellipsoid_label();

    /// Compute the top or bottom face and placement
    void compute_top_bottom_face(faces_mask_type face_id_,
                                 elliptical_sector & face_,
                                 placement & face_placement_) const;

    /// Compute the side face and placement
    void compute_side_face(ellipsoid_sector & face_,
                           placement & face_placement_) const;

    unsigned int compute_faces(face_info_collection_type & faces) const override;

    double get_xmin() const override;

    double get_xmax() const override;

    double get_ymin() const override;

    double get_ymax() const override;

    double get_zmin() const override;

    double get_zmax() const override;

    double get_x_radius() const;

    void set_x_radius(double);

    double get_y_radius() const;

    void set_y_radius(double);

    double get_z_radius() const;

    void set_z_radius(double);

    double get_bottom_z_cut() const;

    void set_bottom_z_cut(double);

    double get_top_z_cut() const;

    void set_top_z_cut(double);

    void set(double rx_, double ry_, double rz_);

    void set(double rx_, double ry_, double rz_,
             double zm_, double zp_);

    /// Constructor
    ellipsoid();

    /// Constructor
    ellipsoid(double rx_, double ry_, double rz_);

    /// Constructor
    ellipsoid(double rx_, double ry_, double rz_,
              double zm_, double zp_);

    /// Destructor
    ~ellipsoid() override;

    /// Return the name of the shape
    std::string get_shape_name() const override;

    /// Return the value of some parameter
    virtual double get_parameter(const std::string &) const;

    /// Check the validity of the ellipsoid
    bool is_valid() const override;

    /// Initialize the ellipsoid from properties
    void initialize(const datatools::properties &,
                            const handle_dict_type * = 0) override;

    /// Reset the ellipsoid
    void reset() override;

    /// Compute the surface
    double get_surface(uint32_t mask_ = FACE_ALL) const override;

    /// Compute the volume
    double get_volume(uint32_t flags_ = 0) const override;

    /// Check if a point is inside the tube
    bool is_inside(const vector_3d &,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the tube
    bool is_outside(const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_bits_any(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the vector normal to the surface at some position
    vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const override;

    /// Find the intercept point with a face of the tube
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    friend std::ostream &
    operator<<(std::ostream &, const ellipsoid &);

    friend std::istream &
    operator>>(std::istream &, ellipsoid &);

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

    /// \brief 3D rendering options
    enum ellipsoid_wires_rendering_option_type {
      WR_ELLIPSOID_NO_SIDE_FACE    = (WR_BASE_LAST << 1),        //!< Do not render the side face
      WR_ELLIPSOID_NO_BOTTOM_FACE  = (WR_BASE_LAST << 2),        //!< Do not render the bottom face
      WR_ELLIPSOID_NO_TOP_FACE     = (WR_BASE_LAST << 3),        //!< Do not render the top face
      WR_ELLIPSOID_LAST            = (WR_ELLIPSOID_NO_TOP_FACE), //!< Last defined bit
      WR_ELLIPSOID_MASK            = (WR_ELLIPSOID_NO_SIDE_FACE
                                      | WR_ELLIPSOID_NO_BOTTOM_FACE
                                      | WR_ELLIPSOID_NO_TOP_FACE
                                      )  //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  protected:

    /// Set default attributes
    void _set_default();

    /// Build bounding data
    void _build_bounding_data() override;

  private:

    double _x_radius_;     //!< x radius
    double _y_radius_;     //!< y radius
    double _z_radius_;     //!< z radius
    double _bottom_z_cut_; //!< lower cut plane level / z
    double _top_z_cut_;    //!< upper cut plane level / z

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(ellipsoid)

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::ellipsoid the name the class
DOCD_CLASS_DECLARATION(geomtools::ellipsoid)

#endif // GEOMTOOLS_ELLIPSOID_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
