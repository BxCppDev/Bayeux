// -*- mode: c++; -*-
/** \file geomtools/box.h */
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2012-10-24
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_BOX_H
#define GEOMTOOLS_BOX_H 1

// Third party:
// -  Boost:
#include <boost/cstdint.hpp>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  /// \brief A box shape
  class box : public i_shape_3d ,
              public i_stackable,
              public i_wires_3d_rendering
  {

  public:

    /// Return the box shape label
    static const std::string & box_label();

    ///  \brief Masks used for the 6 faces of the box
    enum faces_mask_type
      {
        FACE_NONE   = geomtools::FACE_NONE,
        FACE_BACK   = 0x1,
        FACE_FRONT  = 0x2,
        FACE_LEFT   = 0x4,
        FACE_RIGHT  = 0x8,
        FACE_BOTTOM = 0x10,
        FACE_TOP    = 0x20,
        FACE_ALL    = (FACE_BACK
                       | FACE_FRONT
                       | FACE_LEFT
                       | FACE_RIGHT
                       | FACE_BOTTOM
                       | FACE_TOP)
      };

  public:

    /// Return the min X coordinates (bounding box)
    double get_xmin() const;

    /// Return the max X coordinates (bounding box)
    double get_xmax() const;

    /// Return the min Y coordinates (bounding box)
    double get_ymin() const;

    /// Return the max Y coordinates (bounding box)
    double get_ymax() const;

    /// Return the min Z coordinates (bounding box)
    double get_zmin() const;

    /// Return the max Z coordinates (bounding box)
    double get_zmax() const;

    /// Return the X dimension
    double get_x () const;

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

  public:

    /// Default constructor
    box();

    /// Constructor
    box(double a_x, double a_y, double a_z);

    /// Destructor
    virtual ~box();

    /// Return the name of the shape
    virtual std::string get_shape_name() const;

    // Check the validity of the object
    bool is_valid() const;

    /// Initialize the box from properties
    void initialize(const datatools::properties &);

    /// Reset
    void reset();

    /// Return the value of some parameter given by name
    double get_parameter(const std::string &) const;

    /// Return the cumulated surface of the box given a list of faces
    virtual double get_surface(uint32_t a_mask = FACE_ALL_BITS) const;

    /// Return the volume of the box
    virtual double get_volume(uint32_t flags_ = 0) const;

    /// Check if a point is inside the box
    virtual bool is_inside(const vector_3d &,
                           double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a point is inside the box
    virtual bool is_outside(const vector_3d &,
                            double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a point is on some surface of the box
    /// If 'skin' < 0 no skin is taken into account.
    virtual bool is_on_surface(const vector_3d & ,
                               int a_mask    = FACE_ALL_BITS ,
                               double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the vector normal to the surface at some position
    virtual vector_3d get_normal_on_surface(const vector_3d & a_position) const;

    /// Print operator
    friend std::ostream & operator<<( std::ostream & , const box & );

    /// Read operator
    friend std::istream & operator>>( std::istream & , box & );

    /// Find the intercept point of a segment with the box
    virtual bool find_intercept(const vector_3d & a_from,
                                const vector_3d & a_direction,
                                intercept_t & a_intercept,
                                double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump(std::ostream & a_out         = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool a_inherit          = false) const;

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    virtual void generate_wires(std::list<polyline_3d> &,
                                const placement &,
                                uint32_t options_ = 0) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  private:

    double _x_; //!< Width (in arbitrary units)
    double _y_; //!< Length (in arbitrary units)
    double _z_; //!< Height (in arbitrary units)

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::box the name the class
DOCD_CLASS_DECLARATION(geomtools::box)

#endif // GEOMTOOLS_BOX_H
