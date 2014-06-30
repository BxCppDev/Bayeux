// -*- mode: c++; -*-
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
#define GEOMTOOLS_PLANE_H 1

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  /// \brief A plane (2D shape) in a 3D space
  class plane : public i_shape_2d
  {
  public:

    static const std::string & plane_label();

    bool is_valid () const;

    void reset ();

    double a () const;

    double b () const;

    double c () const;

    double d () const;

    geomtools::vector_3d normal () const;

    void set (double a_, double b_, double c_, double d_);

    void set (const geomtools::vector_3d & point_,
              const geomtools::vector_3d & normal_);

    void set (const geomtools::vector_3d & point_,
              const geomtools::vector_3d & u1_,
              const geomtools::vector_3d & u2_);

    plane ();

    plane (double a_, double b_, double c_, double d_);

    plane (const geomtools::vector_3d & point_,
           const geomtools::vector_3d & normal_);

    plane (const geomtools::vector_3d & point_,
           const geomtools::vector_3d & u1_,
           const geomtools::vector_3d & u2_);

    double distance (const geomtools::vector_3d & position_) const;

    geomtools::vector_3d orthogonal_projection (const geomtools::vector_3d & position_) const;

    geomtools::vector_3d projection (const geomtools::vector_3d & position_,
                                     const geomtools::vector_3d & direction_) const;

    virtual bool is_on_surface (const geomtools::vector_3d & position_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual geomtools::vector_3d get_normal_on_surface (const geomtools::vector_3d & position_,
                                                        bool up_ = true) const;

    virtual bool find_intercept (const geomtools::vector_3d & from_,
                                 const geomtools::vector_3d & direction_,
                                 geomtools::intercept_t & intercept_,
                                 double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    void print_grid (std::ostream & out_, double padding_ = 1.0, int n1_ = 3, int n2_ = 3) const;

    virtual std::string get_shape_name () const;

  private:

    double _a_, _b_, _c_, _d_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_PLANE_H
