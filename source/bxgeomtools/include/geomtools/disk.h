// -*- mode: c++ ; -*-
/// \file geomtools/disk.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 *
 * License:
 *
 * Description:
 *   A disk in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_DISK_H
#define GEOMTOOLS_DISK_H 1

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /// \brief A disk (2D shape) in a 3D space
  class disk : public i_shape_2d
  {

  public:

    /// Return the identifier label for the class
    static const std::string & disk_label();

    /// Check if the disk is valid
    bool is_valid() const;

    /// Return the radius
    double get_r() const;

    /// Return the radius
    double get_radius() const;

    /// Set the radius
    void set_r(double);

   /// Set the diameter
     void set_diameter(double);

    /// Return the diameter
    double get_diameter() const;

   /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

   /// Return the perimeter
    virtual double get_perimeter(uint32_t flags_ = ALL_PIECES) const;

   /// Return the circumference
    double get_circumference() const;

    /// Default constructor
    disk();

    /// Constructor
    disk(double r_);

    /// Destructor
    virtual ~disk();

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    virtual bool is_on_surface(const vector_3d &,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                             bool up_ = true) const;

    virtual bool find_intercept(const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual void tree_dump(std::ostream & out_ = std::clog,
                            const std::string & title_ = "",
                            const std::string & indent_ = "",
                            bool inherit_= false) const;

    virtual void generate_wires(std::list<polyline_3d> &,
                                 const placement & ,
                                 uint32_t options_ = 0) const;

  private:

    double _radius_; /// Radius of the disk

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_DISK_H
