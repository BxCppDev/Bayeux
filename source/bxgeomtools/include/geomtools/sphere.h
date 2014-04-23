// -*- mode: c++; -*-
/// \file geomtools/sphere.h
/* Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-28
 * Last modified: 2014-04-08
 *
 * License:
 *
 * Description:
 *   3D sphere description
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SPHERE_H
#define GEOMTOOLS_SPHERE_H 1

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

  class sphere : public i_shape_3d, public i_stackable
  {
  public:

    enum faces_mask_type {
      FACE_NONE             = geomtools::FACE_NONE,
      FACE_SIDE             = datatools::bit_mask::bit00,
      FACE_OUTER_SIDE       = FACE_SIDE,
      FACE_INNER_SIDE       = datatools::bit_mask::bit01,
      FACE_START_THETA_SIDE = datatools::bit_mask::bit02,
      FACE_STOP_THETA_SIDE  = datatools::bit_mask::bit03,
      FACE_START_PHI_SIDE   = datatools::bit_mask::bit04,
      FACE_STOP_PHI_SIDE    = datatools::bit_mask::bit05,
      FACE_ALL    = FACE_OUTER_SIDE | FACE_INNER_SIDE
      | FACE_START_THETA_SIDE | FACE_STOP_THETA_SIDE
      | FACE_START_PHI_SIDE | FACE_STOP_PHI_SIDE
    };

    static const std::string & sphere_label();

    double get_xmin() const;

    double get_xmax() const;

    double get_ymin() const;

    double get_ymax() const;

    double get_zmin() const;

    double get_zmax() const;

    double get_r() const;

    double get_r_max() const;

    double get_radius() const;

    bool has_r_min() const;

    double get_r_min() const;

    void set_r(double);

    void set_r_max(double);

    void set_radius(double);

    void set_r_min(double);

    void reset_r_max();

    void reset_r_min();

    void set(double rmax_);

    void set(double rmin_, double rmax_);

    void set_phi(double start_phi_, double delta_phi_);

    double get_start_phi() const;

    double get_delta_phi() const;

    bool has_partial_phi() const;

    void set_theta(double start_theta_, double delta_theta_);

    double get_start_theta() const;

    double get_delta_theta() const;

    bool has_partial_theta() const;

    bool is_orb() const;

    /// Constructor
    sphere();

    /// Constructor
    sphere(double radius_);

    /// Constructor
    sphere(double radius_min_, double radius_max_);

    /// Destructor
    virtual ~sphere();

    /// Return the name of the shape
    virtual std::string get_shape_name() const;

    /// Return the value of some parameter
    virtual double get_parameter(const std::string &) const;

    /// Check the validity of the sphere
    bool is_valid() const;

    /// Initialize the sphere from properties
    void initialize(const datatools::properties &);

    /// Reset the sphere
    void reset();

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
    operator<<(std::ostream &, const sphere &);

    friend std::istream &
    operator>>(std::istream &, sphere &);

    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_default();

  private:

    double _r_;           /// Outer radius
    double _r_min_;       /// Inner radius
    double _start_phi_;   /// Start phi angle
    double _delta_phi_;   /// Delta phi angle
    double _start_theta_; /// Start theta angle
    double _delta_theta_; /// Delta theta angle

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::sphere the name the class
DOCD_CLASS_DECLARATION(geomtools::sphere)

#endif // GEOMTOOLS_SPHERE_H
