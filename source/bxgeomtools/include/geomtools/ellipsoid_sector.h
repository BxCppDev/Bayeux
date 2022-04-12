/// \file geomtools/ellipsoid_sector.h
/* Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-01
 * Last modified: 2015-03-01
 *
 * License:
 *
 * Description:
 *   3D ellipsoid description
 *
 */

#ifndef GEOMTOOLS_ELLIPSOID_SECTOR_H
#define GEOMTOOLS_ELLIPSOID_SECTOR_H 1

// Standard library:
#include <string>
#include <iostream>

// Third party
// - Bayeux/datatools:
#include <datatools/bit_mask.h>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/angular_range.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  /// \brief The 2D shape/surface model for an ellipsoid sector
  class ellipsoid_sector
    : public i_shape_2d
  {
  public:

    /// Return the identifier label for the class
    static const std::string & ellipsoid_sector_label();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

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

    /// Return the angle domain
    const angular_range & get_angle_domain() const;

    /// Check if the sector has partial angle
    bool has_partial_angle() const;

    /// Check the start angle
    bool has_start_angle() const;

    /// Set the start angle
    void set_start_angle(double);

    /// Return the start angle
    double get_start_angle() const;

    /// Check the delta angle
    bool has_delta_angle() const;

    /// Set the delta angle
    void set_delta_angle(double);

    /// Return the delta angle
    double get_delta_angle() const;

    /// Constructor
    ellipsoid_sector();

    /// Constructor
    ellipsoid_sector(double rx_, double ry_, double rz_);

    /// Constructor
    ellipsoid_sector(double rx_, double ry_, double rz_,
                     double zm_, double zp_);

    /// Constructor
    ellipsoid_sector(double rx_, double ry_, double rz_,
                     double zm_, double zp_,
                     double start_angle_, double delta_angle_);

    /// Destructor
    ~ellipsoid_sector() override;

    /// Check the validity of the ellipsoid sector
    bool is_valid() const override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = nullptr) override;

    /// Reset the ellipsoid sector
    void reset() override;

    /// Return the surface
    double get_surface(uint32_t mask_ = ALL_PIECES) const override;

    /// Check if a point is on the surface
    bool is_on_surface(const vector_3d & ,
                       double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the normal at a given position of the surface
    vector_3d get_normal_on_surface(const vector_3d & position_,
                                    bool check_ = true,
                                    double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Find the intercept point of a segment with the surface
    bool find_intercept(const vector_3d & from_,
                        const vector_3d & direction_,
                        face_intercept_info & intercept_,
                        double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_               = false) const override;

    /// \brief 3D rendering options
    enum ellsec_wires_rendering_option_type {
                                             WR_ELLSEC_NO_BOTTOM_EDGE      = (WR_BASE_LAST << 1),        //!< Do not render the bottom edge
                                             WR_ELLSEC_NO_TOP_EDGE         = (WR_BASE_LAST << 2),        //!< Do not render the top edge
                                             WR_ELLSEC_NO_START_ANGLE_EDGE = (WR_BASE_LAST << 3),        //!< Do not render the start angle edge
                                             WR_ELLSEC_NO_STOP_ANGLE_EDGE  = (WR_BASE_LAST << 4),        //!< Do not render the stop angle edge
                                             WR_ELLSEC_LAST                = (WR_ELLSEC_NO_STOP_ANGLE_EDGE), //!< Last defined bit
                                             WR_ELLIPSOID_MASK             = (WR_ELLSEC_NO_BOTTOM_EDGE
                                                                              | WR_ELLSEC_NO_START_ANGLE_EDGE
                                                                              | WR_ELLSEC_NO_STOP_ANGLE_EDGE
                                                                              )  //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                             uint32_t options_ = 0) const override;

  protected:

    /// Set default attributes
    void _set_defaults();

  private:

    double _x_radius_;     //!< x radius
    double _y_radius_;     //!< y radius
    double _z_radius_;     //!< z radius
    angular_range _angle_domain_; //!< Angular spread of the ellipsoid sector (azimuthal)
    double _bottom_z_cut_; //!< lower cut plane level / z
    double _top_z_cut_;    //!< upper cut plane level / z

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(ellipsoid_sector)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ELLIPSOID_SECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
