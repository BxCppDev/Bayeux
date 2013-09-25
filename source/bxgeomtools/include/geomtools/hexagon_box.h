// -*- mode: c++; -*-
/* hexagon_box.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_HEXAGON_BOX_H_
#define GEOMTOOLS_HEXAGON_BOX_H_ 1

#include <sstream>
#include <stdexcept>

#include <geomtools/i_shape_3d.h>

namespace geomtools {

  class hexagon_box : public i_shape_3d
  {

  public:

    static const std::string & hexagon_box_label();

    /*
     *
     *              |
     *              |
     *            back
     *              |
     *  back left  ---  back right
     *       _____/_|_\______ y
     * front left \ | / front right
     *             ---
     *              |
     *            front
     *              |
     *            x |
     *
     */

    enum faces_mask_type
      {
        FACE_NONE         = geomtools::FACE_NONE,
        FACE_BACK         = datatools::bit_mask::bit00,
        FACE_FRONT        = datatools::bit_mask::bit01,
        FACE_FRONT_LEFT   = datatools::bit_mask::bit02,
        FACE_FRONT_RIGHT  = datatools::bit_mask::bit03,
        FACE_BACK_LEFT    = datatools::bit_mask::bit04,
        FACE_BACK_RIGHT   = datatools::bit_mask::bit05,
        FACE_BOTTOM       = datatools::bit_mask::bit06,
        FACE_TOP          = datatools::bit_mask::bit07,
        FACE_ALL    = (FACE_BACK
                       | FACE_FRONT
                       | FACE_BACK_LEFT
                       | FACE_BACK_RIGHT
                       | FACE_FRONT_LEFT
                       | FACE_FRONT_RIGHT
                       | FACE_BOTTOM
                       | FACE_TOP)
      };


  public:

    double get_radius () const;

    double get_side () const;

    double get_diameter () const;

    void set_radius (double);

    double get_z () const;

    void set_z (double);

    double get_half_z () const;

    void set_half_z (double);

    void set (double radius_, double z_);

    // ctor/dtor:
  public:
    hexagon_box ();

    hexagon_box (double, double);

    virtual ~hexagon_box ();

    // methods:

    virtual std::string get_shape_name () const;

    double get_parameter (const std::string &) const;

    bool is_valid () const;

    void init ();

    void reset ();

    virtual double get_surface (uint32_t mask_ = FACE_ALL) const;

    virtual double get_volume (uint32_t flags = 0) const;

    virtual bool is_inside (const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool is_on_surface (const vector_3d & ,
                                int mask_    = FACE_ALL ,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    friend std::ostream & operator<< (std::ostream & , const hexagon_box &);

    friend std::istream & operator>> (std::istream & , hexagon_box &);

    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    static bool xy_is_in_hexagon (double r_, double x_, double y_, double skin_);

    static bool xy_is_out_hexagon (double r_, double x_, double y_, double skin_);

    static bool xy_is_on_hexagon (double r_, double x_, double y_, double skin_);

  private:

    double _radius_;
    double _z_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_HEXAGON_BOX_H_

// end of hexagon_box.h

