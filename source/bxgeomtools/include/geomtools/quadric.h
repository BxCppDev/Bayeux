//! \file  geomtools/quadric.h
//! \brief Quadric surface
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of geomtools.
//
// geomtools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// geomtools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with geomtools. If not, see <http://www.gnu.org/licenses/>.

#ifndef GEOMTOOLS_QUADRIC_H
#define GEOMTOOLS_QUADRIC_H

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  //! \brief A general quadric a 3D space
  class quadric : public i_shape_2d
  {
  public:

    //! Return the identifier label for the class
    static const std::string & quadric_label();

    //! Return the shape name
    std::string get_shape_name() const override;

    //! Check if the plane is valid
    bool is_valid() const override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    //! Reset
    void reset() override;

    //! Default constructor
    quadric();

    enum coef_index {
      A = 9,
      B = 8,
      C = 7,
      D = 6,
      E = 5,
      F = 4,
      G = 3,
      H = 2,
      I = 1,
      J = 0,
      NCOEFFS = 10
    };

    static std::string coeff_label(coef_index i_);

    //! Set the coefficient at given rank
    void set_coefficient(coef_index, double);

    //! Return the coefficient at given rank
    double get_coefficient(coef_index) const;

    //! Compute the quadric formula f(x,y,z) at given 3D point
    virtual double eval_f(double x_, double y_, double z_) const;

    //! Compute the quadric df/dx derivative at given 3D point
    virtual double eval_dfdx(double x_, double y_, double z_) const;

    //! Compute the quadric df/dy derivative at given 3D point
    virtual double eval_dfdy(double x_, double y_, double z_) const;

    //! Compute the quadric df/dz derivative at given 3D point
    virtual double eval_dfdz(double x_, double y_, double z_) const;

    //! Compute the quadric gradient (df/dx, df/dy, df/dz) at given 3D point
    void eval_gradient(double x_, double y_, double z_,
                       double & dfdx_, double & dfdy_, double & dfdz_) const;

    //! Compute the quadric gradient (df/dx, df/dy, df/dz) at given 3D point
    void eval_gradient(const geomtools::vector_3d & position_,
                       geomtools::vector_3d & gradient_) const;

    //! Compute the quadric formula at given 3D point
    double eval_f(const geomtools::vector_3d & position_) const;

    /// Compute the distance from a position to the plane
    double distance(const geomtools::vector_3d & position_) const;

    //! Compute the orthogonal projection from a position
    geomtools::vector_3d orthogonal_projection(const geomtools::vector_3d & position_,
                                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    //! Compute the projection from a position along a direction
    geomtools::vector_3d projection(const geomtools::vector_3d & position_,
                                    const geomtools::vector_3d & direction_,
                                    double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    //! Check if the point
    bool is_on_surface(const geomtools::vector_3d & position_,
                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    geomtools::vector_3d
    get_normal_on_surface(const geomtools::vector_3d & position_,
                          bool check_ = true,
                          double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    bool find_intercept(const geomtools::vector_3d & from_,
                                const geomtools::vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    //! Make a general plane
    //! f(x,y,z) = ax + by + cz + d = 0
    static void make_plane(quadric & q_, double a_, double b_, double c_, double d_);

    //! Make a YZ plane at x=a
    //! f(x,y,z) = x - a = 0
    static void make_plane_yz(quadric & q_, double a_);

    //! Make a XZ plane at y=b
    //! f(x,y,z) = y - b = 0
    static void make_plane_xz(quadric & q_, double b_);

    //! Make a XY plane at z=c
    //! f(x,y,z) = z - c = 0
    static void make_plane_xy(quadric & q_, double c_);

    //! Make parallel YZ planes at x2=a2
    //! f(x,y,z) = x2 - a2 = 0
    static void make_parallel_planes_yz(quadric & q_, double a_);

    //! Make parallel XZ planes at y2=b2
    //! f(x,y,z) = y2 - b2 = 0
    static void make_parallel_planes_xz(quadric & q_, double b_);

    //! Make parallel XY planes at z2=c2
    //! f(x,y,z) = z2 - c2 = 0
    static void make_parallel_planes_xy(quadric & q_, double c_);

    //! Make ellipsoid
    //! f(x,y,z) = (x/a)2 + (y/b)2 + (z/c)2 - 1 = 0
    static void make_ellipsoid(quadric & q_, double a_, double b_, double c_);

    //! Make spheroid along Z axis
    //! f(x,y,z) = (x/a)2 + (y/a)2 + (z/b)2 - 1 = 0
    static void make_spheroid_along_z(quadric & q_, double a_, double b_);

    //! Make sphere
    //! f(x,y,z) = (x/a)2 + (y/a)2 + (z/a)2 - 1 = 0
    static void make_sphere(quadric & q_, double a_);

    //! Make elliptic paraboloid along Z axis
    //! f(x,y,z) = (x/a)2 + (y/b)2 - z = 0
    static void make_elliptic_paraboloid_along_z(quadric & q_, double a_, double b_);

    //! Make circular paraboloid along Z axis
    //! f(x,y,z) = (x/a)2 + (y/a)2 - z = 0
    static void make_circular_paraboloid_along_z(quadric & q_, double a_);

    //! Make hyperbolic paraboloid along Z axis
    //! f(x,y,z) = (x/a)2 - (y/b)2 - z = 0
    static void make_hyperbolic_paraboloid_along_z(quadric & q_, double a_, double b_);

    //! Make hyperbolic paraboloid of one sheet along Z axis
    //! f(x,y,z) = (x/a)2 + (y/b)2 - (z/c)2 - 1 = 0
    static void make_elliptic_hyperbolic_one_sheet_along_z(quadric & q_, double a_, double b_, double c_);

    //! Make hyperbolic paraboloid of two sheets along Z axis
    //! f(x,y,z) = (x/a)2 + (y/b)2 - (z/c)2 + 1 = 0
    static void make_elliptic_hyperbolic_two_sheets_along_z(quadric & q_, double a_, double b_, double c_);

    //! Make elliptic cone along Z axis
    //! f(x,y,z) = (x/a)2 + (y/b)2 - (z/c)2 = 0
    static void make_elliptic_cone_along_z(quadric & q_, double a_, double b_, double c_);

    //! Make circular cone along Z axis
    //! f(x,y,z) = (x/a)2 + (y/a)2 - (z/b)2 = 0
    static void make_circular_cone_along_z(quadric & q_, double a_, double b_);

    //! Make elliptic cylinder along Z axis
    //! f(x,y,z) = (x/a)2 + (y/b)2 - 1 = 0
    static void make_elliptic_cylinder_along_z(quadric & q_, double a_, double b_);

    //! Make circular cylinder along Z axis
    //! f(x,y,z) = (x/a)2 + (y/a)2 - 1 = 0
    static void make_circular_cylinder_along_z(quadric & q_, double a_);

    //! Make hyperbolic cylinder along Z axis
    //! f(x,y,z) = (x/a)2 - (y/b)2 - 1 = 0
    static void make_hyperbolic_cylinder_along_z(quadric & q_, double a_, double b_);

    //! Make parabolic cylinder along Z axis
    //! f(x,y,z) = x2 + 2ay = 0
    static void make_parabolic_cylinder_along_z(quadric & q_, double a_);

    //!
    void generate_wires_self(wires_type &, uint32_t options_ = 0) const override;

    //! Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const override;

  protected:

    //! Set default attributes values
    void _set_defaults();

  private:

    double _coefficients_[NCOEFFS]; //!< Coefficients of the quadric

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(quadric)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_QUADRIC_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
