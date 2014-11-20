// -*- mode: c++; -*-
/// \file geomtools/elliptical_tube.h
/* Author(s):     A. Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2014-11-17
 * Last modified: 2014-11-17
 *
 * License:
 *
 * Description:
 *   3D elliptical_tube description
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_ELLIPTICAL_TUBE_H
#define GEOMTOOLS_ELLIPTICAL_TUBE_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward class declaration:
  class cylinder;

  /// \brief The 3D shape model for an elliptical_tube
  class elliptical_tube : public i_shape_3d, public i_stackable
  {
  public:
    enum faces_mask_type
      {
        FACE_NONE   = geomtools::FACE_NONE,
        FACE_SIDE   = datatools::bit_mask::bit00,
        FACE_BOTTOM = datatools::bit_mask::bit01,
        FACE_TOP    = datatools::bit_mask::bit02,
        FACE_ALL    = (FACE_SIDE
                       | FACE_BOTTOM
                       | FACE_TOP)
      };

  public:

    static const std::string & elliptical_tube_label();

  public:

    double get_xmin () const;

    double get_xmax () const;

    double get_ymin () const;

    double get_ymax () const;

    double get_zmin () const;

    double get_zmax () const;

    double get_z () const;

    void set_z (double);

    double get_half_z() const;

    void set_half_z (double);

    double get_x_radius () const;

    void set_x_radius (double);

    double get_y_radius () const;

    void set_y_radius (double);

    void set (double xr_, double yr_, double z_);

  public:

    /// Default constructor
    elliptical_tube();

    /// Constructor
    elliptical_tube (double xr_, double yr_, double z_);

    /// Destructor
    virtual ~elliptical_tube ();

    virtual std::string get_shape_name () const;

    virtual double get_parameter (const std::string &) const;

    bool is_valid () const;

    /// Initialize from properties
    virtual void initialize(const datatools::properties &,
                            const handle_dict_type *);

    /// Reset
    virtual void reset();

    virtual double get_surface (uint32_t mask_ = FACE_ALL_BITS) const;

    virtual double get_volume (uint32_t flags_ = 0) const;

    virtual bool is_inside (const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_outside (const vector_3d &,
                             double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_on_surface (const vector_3d &,
                                int mask_    = FACE_ALL,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    friend std::ostream &
    operator<< (std::ostream &, const elliptical_tube &);

    friend std::istream &
    operator>> (std::istream &, elliptical_tube &);

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    virtual void generate_wires (std::list<polyline_3d> &,
                                 const placement & ,
                                 uint32_t options_ = 0) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_default();

    virtual void _build_bounding_data();

  private:

    double _z_;
    double _x_radius_;
    double _y_radius_;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(elliptical_tube);

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::elliptical_tube the name the class
DOCD_CLASS_DECLARATION(geomtools::elliptical_tube)

#endif // GEOMTOOLS_ELLIPTICAL_TUBE_H
