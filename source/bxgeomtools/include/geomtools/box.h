/// \file geomtools/box.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2021-04-24
 *
 * Description:
 *
 *  Box 3D solid shape
 *
 */

#ifndef GEOMTOOLS_BOX_H
#define GEOMTOOLS_BOX_H 1

// Standard library:
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
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

  // Class forward declarations:
  class rectangle;

  /// \brief The 3D shape model for a box
  class box : public i_shape_3d ,
              public i_stackable
  {

  public:

    /// Return the box shape label
    static const std::string & box_label();

    /// \brief Masks used for the 6 faces of the box
    enum faces_mask_type
      {
       FACE_NONE   = face_identifier::FACE_BITS_NONE,
       _FACE_BEGIN = 0x1,
       FACE_BACK   = 0x1,
       FACE_FRONT  = 0x2,
       FACE_LEFT   = 0x4,
       FACE_RIGHT  = 0x8,
       FACE_BOTTOM = 0x10,
       FACE_TOP    = 0x20,
       _FACE_END   = 0x40,
       FACE_ALL    = (FACE_BACK
                      | FACE_FRONT
                      | FACE_LEFT
                      | FACE_RIGHT
                      | FACE_BOTTOM
                      | FACE_TOP)
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
    void set(double, double, double);

    /// Set the halves of all dimensions
    void set_half(double, double, double);

    /// Default constructor
    box();

    /// Constructor
    box(double a_x, double a_y, double a_z);

    /// Destructor
    ~box() override;

    /// Return the name of the shape
    std::string get_shape_name() const override;

    // Check the validity of the object
    bool is_valid() const override;

    /// Initialize the box from properties
    void initialize(const datatools::properties &, const handle_dict_type * = nullptr) override;

    /// Reset
    void reset() override;

    /// Return the value of some parameter given by name
    double get_parameter(const std::string &) const;

    /// Return the cumulated surface of the box given a list of faces
    double get_surface(uint32_t a_mask = FACE_ALL) const override;

    /// Return the volume of the box
    double get_volume(uint32_t flags_ = 0) const override;

    /// Return a collection of face info objects
    unsigned int compute_faces(face_info_collection_type & faces_) const override;

    /// Compute a deflated version of the box
    void compute_deflated(box & deflated_,
                          double by_x_,
                          double by_y_,
                          double by_z_) const;

    /// Compute an inflated version of the box
    void compute_inflated(box & deflated_,
                          double by_x_,
                          double by_y_,
                          double by_z_) const;

    /// Compute an envelope box at user tolerance
    void compute_envelope(box & envelope_,
                          double tolerance_x_,
                          double tolerance_y_,
                          double tolerance_z_) const;

    /// Check if a point is inside the box
    bool is_inside(const vector_3d &,
                   double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is inside the box
    bool is_outside(const vector_3d &,
                    double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                               const face_identifier & a_surface_mask = face_identifier::face_bits_any(),
                               double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the vector normal to the surface at some position
    vector_3d get_normal_on_surface(const vector_3d & a_position,
                                    const face_identifier & a_surface_bit) const override;

    /// Print operator
    friend std::ostream & operator<<( std::ostream & , const box & );

    /// Read operator
    friend std::istream & operator>>( std::istream & , box & );

    /// Find the intercept point of a segment with the box
    bool find_intercept(const vector_3d & a_from,
                        const vector_3d & a_direction,
                        face_intercept_info & a_intercept,
                        double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & a_out         = std::clog,
                   const std::string & a_title  = "",
                   const std::string & a_indent = "",
                   bool a_inherit          = false) const override;

    /// \brief 3D rendering options
    enum box_wires_rendering_option_type {
                                          WR_BOX_NO_BACK_FACE   = (WR_BASE_LAST << 1), //!< Do not render the X- (back) face
                                          WR_BOX_NO_FRONT_FACE  = (WR_BASE_LAST << 2), //!< Do not render the X+ (front) face
                                          WR_BOX_NO_LEFT_FACE   = (WR_BASE_LAST << 3), //!< Do not render the Y- (left) face
                                          WR_BOX_NO_RIGHT_FACE  = (WR_BASE_LAST << 4), //!< Do not render the Y+ (right) face
                                          WR_BOX_NO_BOTTOM_FACE = (WR_BASE_LAST << 5), //!< Do not render the Z- (bottom) face
                                          WR_BOX_NO_TOP_FACE    = (WR_BASE_LAST << 6), //!< Do not render the Z+ (top) face
                                          WR_BOX_LAST           = (WR_BOX_NO_TOP_FACE),    //!< Last defined bit
                                          WR_BOX_MASK           = (WR_BOX_NO_BACK_FACE
                                                                   | WR_BOX_NO_FRONT_FACE
                                                                   | WR_BOX_NO_LEFT_FACE
                                                                   | WR_BOX_NO_RIGHT_FACE
                                                                   | WR_BOX_NO_BOTTOM_FACE
                                                                   | WR_BOX_NO_TOP_FACE)   //!< Rendering options bit mask
    };

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    void generate_wires_self(wires_type & wires_,
                             uint32_t options_ = 0) const override;

    /// Compute the positions of the 8 vertexes of the box
    void compute_vertexes(std::vector<vector_3d> & vv_) const;

    /// Compute the positions of the 8 vertexes in a given transformation
    void compute_transformed_vertexes(const placement & p_, std::vector<vector_3d> & vv_) const;

    /// Compute an arbitrary face
    void compute_face(faces_mask_type face_id_,
                      rectangle & face_,
                      placement & face_placement_) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_defaults();

    /// Build the bounding data
    void _build_bounding_data() override;

  private:

    double _x_; //!< Width (in arbitrary units)
    double _y_; //!< Length (in arbitrary units)
    double _z_; //!< Height (in arbitrary units)

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(box)

  };

} // end of namespace geomtools

/// OCD support declaration
// @param geomtools::box the name of the class with registered OCD support
DOCD_CLASS_DECLARATION(geomtools::box)

#endif // GEOMTOOLS_BOX_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
