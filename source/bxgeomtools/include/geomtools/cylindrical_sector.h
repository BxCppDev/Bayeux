/// \file geomtools/cylindrical_sector.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-26
 * Last modified: 2016-03-10
 *
 * License:
 *
 * Description:
 *
 *   Cylindrical sector
 *
 */

#ifndef GEOMTOOLS_CYLINDRICAL_SECTOR_H
#define GEOMTOOLS_CYLINDRICAL_SECTOR_H 1

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/angular_range.h>

namespace geomtools {

  /// \brief A spherical sector (2D shape)
  class cylindrical_sector : public i_shape_2d
  {
  public:

    /// Return the identifier label for the class
    static const std::string & cylindrical_sector_label();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Set the radius
    void set_radius(double new_value_);

    /// Return the radius
    double get_radius() const;

    /// Set the height
    void set_z(double new_value_);

    /// Return the height
    double get_z() const;

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

    /// Set the radius and angular bounds
    void set(double radius_,
             double z_,
             double start_angle_,
             double delta_angle_);

    /// Default constructor
    cylindrical_sector();

    /// Constructor
    cylindrical_sector(double radius_,
                       double z_);

    /// Constructor
    cylindrical_sector(double radius_,
                       double z_,
                       double start_angle_,
                       double delta_angle_);

    /// Destructor
    ~cylindrical_sector() override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset
    void reset() override;

    /// Check if the rectangle is valid
    bool is_valid() const override;

    /// Return the surface
    double get_surface(uint32_t flags_ = ALL_PIECES) const override;

    /// Check if a point is on the surface
    bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the normal at a given position of the surface
    vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = true,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Find the intercept point of a segment with the surface
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const override;

    /// \brief 3D rendering options
    enum cylsec_wires_rendering_option_type {
      WR_CYLSEC_NO_BOTTOM_EDGE      = (WR_BASE_LAST << 1),             //!< Do not render the bottom edge
      WR_CYLSEC_NO_TOP_EDGE         = (WR_BASE_LAST << 2),             //!< Do not render the top edge
      WR_CYLSEC_NO_START_ANGLE_EDGE = (WR_BASE_LAST << 3),             //!< Do not render the start angle edge
      WR_CYLSEC_NO_STOP_ANGLE_EDGE  = (WR_BASE_LAST << 4),             //!< Do not render the stop angle edge
      WR_CYLSEC_LAST                = (WR_CYLSEC_NO_STOP_ANGLE_EDGE),  //!< Last defined bit
      WR_CYLSEC_MASK                = (WR_CYLSEC_NO_BOTTOM_EDGE
                                       | WR_CYLSEC_NO_TOP_EDGE
                                       | WR_CYLSEC_NO_START_ANGLE_EDGE
                                       | WR_CYLSEC_NO_STOP_ANGLE_EDGE) //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  protected:

    /// Set default attributes
    void _set_defaults();

  private:

    double        _radius_;       //!< The radius
    double        _z_;            //!< The height
    angular_range _angle_domain_; //!< Angular spread of the cylindrical sector

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_CYLINDRICAL_SECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
