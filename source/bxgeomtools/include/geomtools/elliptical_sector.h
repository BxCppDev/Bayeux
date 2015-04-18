/// \file geomtools/elliptical_sector.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2015-02-15
 *
 * License:
 *
 * Description:
 *
 *   A elliptical_sector in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_ELLIPTICAL_SECTOR_H
#define GEOMTOOLS_ELLIPTICAL_SECTOR_H 1

// This project:
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  // Forward class declaration:
  class ellipse;

  /// \brief A elliptical_sector sector surface (2D shape)
  class elliptical_sector : public i_shape_2d
  {

  public:

    /// Return the identifier label for the class
    static const std::string & elliptical_sector_label();

    /// Check if the elliptical_sector is valid
    bool is_valid() const;

    /// Return the X radius
    double get_x_radius() const;

    /// Return the Y radius
    double get_y_radius() const;

    /// Set the radii
    void set_radii(double x_radius_, double y_radius_);

    /// Set the X radius
    void set_x_radius(double x_radius_);

    /// Set the Y radius
    void set_y_radius(double y_radius_);

    /// Check for the partial arc
    bool has_partial_angle() const;

    /// Check the start angle
    bool has_start_angle() const;

    /// Set the start angle
    void set_start_angle(double);

    /// Return the start angle
    double get_start_angle() const;

    /// Check the dedislta angle
    bool has_delta_angle() const;

    /// Set the delta angle
    void set_delta_angle(double);

    /// Return the delta angle
    double get_delta_angle() const;

    /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Return the perimeter
    virtual double get_perimeter(uint32_t flags_ = ALL_PIECES) const;

    /// Return the circumference
    double get_circumference() const;

    /// Compute the external edge
    void compute_edge(ellipse & edge_) const;

    // Set the parameters
    void set(double x_radius_, double y_radius_);

    // Set the parameters
    void set(double x_radius_, double y_radius_, double start_angle_, double delta_angle_);

    /// Default constructor
    elliptical_sector();

    /// Constructor
    elliptical_sector(double x_radius_, double y_radius_);

    /// Constructor
    elliptical_sector(double x_radius_, double y_radius_,
                      double start_angle_, double delta_angle_);

    /// Destructor
    virtual ~elliptical_sector();

    /// Reset
    void reset();

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    /// Check if a point is on the surface
    virtual bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the normal at a given position of the surface
    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = true,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Find some intercept point on the surface
    virtual bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const;

    /// \brief 3D rendering options
    enum elliptical_sector_wires_rendering_option_type {
      WR_ELLIPTICAL_SECTOR_NO_EXT_EDGE         = (WR_BASE_LAST << 1),           //!< Do not render the external edge
      WR_ELLIPTICAL_SECTOR_NO_START_ANGLE_EDGE = (WR_BASE_LAST << 2),           //!< Do not render the start angle edge
      WR_ELLIPTICAL_SECTOR_NO_STOP_ANGLE_EDGE  = (WR_BASE_LAST << 3),           //!< Do not render the stop angle edge
      WR_ELLIPTICAL_SECTOR_LAST                = (WR_ELLIPTICAL_SECTOR_NO_STOP_ANGLE_EDGE),  //!< Last defined bit
      WR_ELLIPTICAL_SECTOR_MASK                = (WR_ELLIPTICAL_SECTOR_NO_EXT_EDGE
                                                  | WR_ELLIPTICAL_SECTOR_NO_START_ANGLE_EDGE
                                                  | WR_ELLIPTICAL_SECTOR_NO_STOP_ANGLE_EDGE) //!< Elliptical sector rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    double _x_radius_;    //!< The X radius of the ellipse (in arbitrary units).
    double _y_radius_;    //!< The Y radius of the ellipse (in arbitrary units).
    double _start_angle_; //!< Start angle of the elliptic arc
    double _delta_angle_; //!< Delta angle of the elliptic arc

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ELLIPTICAL_SECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
