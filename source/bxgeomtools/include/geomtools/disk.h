/// \file geomtools/disk.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2015-02-15
 *
 * License:
 *
 * Description:
 *
 *   A disk in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_DISK_H
#define GEOMTOOLS_DISK_H 1

// This project:
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  /// \brief A disk sector surface (2D shape)
  class disk : public i_shape_2d
  {

  public:

    /// Return the identifier label for the class
    static const std::string & disk_label();

    /// Check if the disk is valid
    bool is_valid() const;

    /// Set the outer radius
    void set_outer_r(double);

    /// Return the outer radius
    double get_outer_r() const;

    /// Set the outer radius
    void set_r(double);

    /// Return the outer radius
    double get_r() const;

    /// Return the outer radius
    double get_radius() const;

    /// Set the outer diameter
    void set_diameter(double);

    /// Return the outer diameter
    double get_diameter() const;

    /// Check the inner radius
    bool has_inner_r() const;

    /// Return the inner radius
    double get_inner_r() const;

    /// Set the inner radius
    void set_inner_r(double);

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

    // Set the parameters
    void set(double inner_r_, double outer_r_);

    // Set the parameters
    void set(double inner_r_, double outer_r_, double start_angle_, double delta_angle_);

    /// Default constructor
    disk();

    /// Constructor
    disk(double outer_r_);

    /// Constructor
    disk(double inner_r_, double outer_r_);

    /// Constructor
    disk(double inner_r_, double outer_r_, double start_angle_, double delta_angle_);

    /// Destructor
    virtual ~disk();

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0);

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
    enum disk_wires_rendering_option_type {
      WR_DISK_NO_EXT_EDGE         = (WR_BASE_LAST << 1),           //!< Do not render the external edge
      WR_DISK_NO_INT_EDGE         = (WR_BASE_LAST << 2),           //!< Do not render the internal edge
      WR_DISK_NO_START_ANGLE_EDGE = (WR_BASE_LAST << 3),           //!< Do not render the start angle edge
      WR_DISK_NO_STOP_ANGLE_EDGE  = (WR_BASE_LAST << 4),           //!< Do not render the stop angle edge
      WR_DISK_LAST                = (WR_DISK_NO_STOP_ANGLE_EDGE),  //!< Last defined bit
      WR_DISK_MASK                = (WR_DISK_NO_EXT_EDGE
                                     | WR_DISK_NO_INT_EDGE
                                     | WR_DISK_NO_START_ANGLE_EDGE
                                     | WR_DISK_NO_STOP_ANGLE_EDGE) //!< Disk rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    double _inner_radius_; //!< Internal radius of the disk sector
    double _outer_radius_; //!< External radius of the disk sector
    double _start_angle_;  //!< Start angle of the disk sector
    double _delta_angle_;  //!< Delta angle of the disk sector

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(disk);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_DISK_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
