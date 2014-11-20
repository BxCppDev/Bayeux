// -*- mode: c++; -*-
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

  /// \brief The 3D shape model for an ellipsoid
  class ellipsoid : public i_shape_3d, public i_stackable
  {
  public:

    enum faces_mask_type {
      FACE_NONE   = geomtools::FACE_NONE,
      FACE_SIDE   = datatools::bit_mask::bit00,
      FACE_TOP    = datatools::bit_mask::bit01,
      FACE_BOTTOM = datatools::bit_mask::bit02,
      FACE_ALL    = FACE_SIDE | FACE_TOP | FACE_BOTTOM
    };

    static const std::string & ellipsoid_label();

    double get_xmin() const;

    double get_xmax() const;

    double get_ymin() const;

    double get_ymax() const;

    double get_zmin() const;

    double get_zmax() const;

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
    virtual ~ellipsoid();

    /// Return the name of the shape
    virtual std::string get_shape_name() const;

    /// Return the value of some parameter
    virtual double get_parameter(const std::string &) const;

    /// Check the validity of the ellipsoid
    bool is_valid() const;

    /// Initialize the ellipsoid from properties
    virtual void initialize(const datatools::properties &,
                            const handle_dict_type * = 0);

    /// Reset the ellipsoid
    virtual void reset();

    virtual double get_surface(uint32_t mask_ = FACE_ALL_BITS) const;

    virtual double get_volume(uint32_t flags_ = 0) const;

    virtual bool is_inside(const vector_3d &,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_outside(const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_on_surface(const vector_3d & ,
                               int mask_    = FACE_ALL ,
                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface(const vector_3d & position_) const;

    virtual bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                intercept_t & intercept_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    friend std::ostream &
    operator<<(std::ostream &, const ellipsoid &);

    friend std::istream &
    operator>>(std::istream &, ellipsoid &);

    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_default();

    virtual void _build_bounding_data();

  private:

    double _x_radius_;     //!< x radius
    double _y_radius_;     //!< y radius
    double _z_radius_;     //!< z radius
    double _bottom_z_cut_; //!< lower cut plane level / z
    double _top_z_cut_;    //!< upper cut plane level / z

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(ellipsoid);

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::ellipsoid the name the class
DOCD_CLASS_DECLARATION(geomtools::ellipsoid)

#endif // GEOMTOOLS_ELLIPSOID_H
