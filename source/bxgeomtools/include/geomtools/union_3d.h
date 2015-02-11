// -*- mode: c++; -*-
/// \file geomtools/union_3d.h
/*
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-24
 * Last modified: 2008-05-24
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_UNION_3D_H
#define GEOMTOOLS_UNION_3D_H 1

// This project:
#include <geomtools/i_composite_shape_3d.h>
// #include <geomtools/i_wires_drawer.h>

namespace geomtools {

  //! \brief Union of two 3D shapes
  class union_3d : public i_composite_shape_3d
  {
  public:

    static const std::string & union_3d_label();

    std::string get_shape_name() const;

    union_3d();

    virtual ~union_3d();

    virtual bool is_inside(const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_outside(const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_on_surface(const vector_3d & position_,
                                int mask_    = COMPONENT_SHAPE_ALL,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface(const vector_3d & position_) const;

    virtual bool find_intercept(const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    // /// Generate a list of polylines representing the contour of the shape (for display clients)
    // virtual void generate_wires(std::list<polyline_3d> &,
    //                             const placement &,
    //                             uint32_t options_ = 0) const;

    // /// \brief Special Gnuplot rendering
    // struct wires_drawer : public i_wires_drawer
    // {
    //   //! Constructor
    //   wires_drawer(const union_3d & eb_);
    //
    //   //! Destructor
    //   virtual ~wires_drawer();
    //
    //   //! Output
    //   virtual void generate_wires(std::ostream & out_,
    //                               const geomtools::vector_3d & position_,
    //                               const geomtools::rotation_3d & rotation_);
    // private:
    //   const union_3d * _union_; //!< Handle to the target union solid
    //   boost::scoped_ptr<std::list<polyline_3d> > _wires_ptr_; //!< Local cache
    // };

  protected:

    virtual void _build_bounding_data();

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(union_3d);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_UNION_3D_H
