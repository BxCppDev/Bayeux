/// \file geomtools/hexagon_box.h
/* Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2021-04-24
 *
 * Description: hexagon box 3D solid shape
 *
 */

#ifndef GEOMTOOLS_HEXAGON_BOX_H
#define GEOMTOOLS_HEXAGON_BOX_H 1

// Standard library:
#include <sstream>
#include <stdexcept>

// This project:
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

  public:

    /// Default constructor
    hexagon_box ();

    /// Constructor
    hexagon_box (double, double);

    /// Destructor
    virtual ~hexagon_box ();

    virtual std::string get_shape_name () const;

    double get_parameter (const std::string &) const;

    bool is_valid () const;

    void init ();

    void reset ();

    virtual double get_surface (uint32_t mask_ = FACE_ALL) const;

    virtual double get_volume (uint32_t flags = 0) const;

    virtual bool is_inside (const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_outside (const vector_3d &,
                             double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

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

    /// Compute a deflated version of the hexagon box
    void compute_deflated(hexagon_box & deflated_,
                          double by_r_,
                          double by_z_);

    /// Compute an inflated version of the hexagon box
    void compute_inflated(hexagon_box & deflated_,
                          double by_r_,
                          double by_z_);

  private:

    double _radius_; ///< Radius of the hexagonal box
    double _z_;      ///< Height of the hexagonal box

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_HEXAGON_BOX_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
