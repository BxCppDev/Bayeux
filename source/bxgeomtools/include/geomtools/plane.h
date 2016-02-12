/// \file geomtools/plane.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-09-27
 * Last modified: 2012-09-27
 *
 * License:
 *
 * Description:
 *
 *  A plane in 3D cartesian space
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_PLANE_H
#define GEOMTOOLS_PLANE_H

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  /// \brief A plane (2D shape) in a 3D space
  class plane : public i_shape_2d
  {
  public:

    /// Return the identifier label for the class
    static const std::string & plane_label();

    /// Check if the plane is valid
    bool is_valid() const;

    /// Reset
    void reset();

    /// Return the first parameter
    double a() const;

    /// Return the second parameter
    double b() const;

    /// Return the third parameter
    double c() const;

    /// Return the fourth parameter
    double d() const;

    /// Return the normal to the plane
    geomtools::vector_3d normal() const;

    /// Set the parameters
    void set(double a_, double b_, double c_, double d_);

    /// Set the parameters from a point and the normal
    void set(const geomtools::vector_3d & point_,
             const geomtools::vector_3d & normal_);

    /// Set the parameters from a point and two vectors
    void set(const geomtools::vector_3d & point_,
             const geomtools::vector_3d & u1_,
             const geomtools::vector_3d & u2_);

    /// Default constructor
    plane();

    /// Constructor
    plane(double a_, double b_, double c_, double d_);

    /// Constructor from a point and the normal
    plane(const geomtools::vector_3d & point_,
          const geomtools::vector_3d & normal_);

    /// Constructor from a point and two vectors
    plane(const geomtools::vector_3d & point_,
          const geomtools::vector_3d & u1_,
          const geomtools::vector_3d & u2_);

    /// Compute the distance from a position to the plane
    double distance(const geomtools::vector_3d & position_) const;

    /// Compute the orthogonal projection from a position
    geomtools::vector_3d orthogonal_projection(const geomtools::vector_3d & position_) const;

    geomtools::vector_3d projection(const geomtools::vector_3d & position_,
                                    const geomtools::vector_3d & direction_) const;

    virtual bool is_on_surface(const geomtools::vector_3d & position_,
                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual geomtools::vector_3d
    get_normal_on_surface(const geomtools::vector_3d & position_,
                          bool check_ = true,
                          double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool find_intercept(const geomtools::vector_3d & from_,
                                const geomtools::vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    void print_grid(std::ostream & out_, double padding_ = 1.0, int n1_ = 3, int n2_ = 3) const;

    virtual std::string get_shape_name() const;

    /// Generate a sequence of polylines for wires 3D rendering
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    double _a_;
    double _b_;
    double _c_;
    double _d_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_PLANE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
