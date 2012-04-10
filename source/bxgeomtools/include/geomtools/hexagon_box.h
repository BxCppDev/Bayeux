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

#ifndef __geomtools__hexagon_box_h
#define __geomtools__hexagon_box_h 1

#include <sstream>
#include <stdexcept>

#include <geomtools/i_shape_3d.h>

namespace geomtools {

  class hexagon_box : public i_shape_3d 
  {

  public:

    static const std::string HEXAGON_BOX_LABEL;

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
        FACE_NONE   = FACE_NONE_BIT,
        FACE_BACK         = 0x1,
        FACE_FRONT        = 0x2,
        FACE_FRONT_LEFT   = 0x4,
        FACE_FRONT_RIGHT  = 0x8,
        FACE_BACK_LEFT    = 0x10,
        FACE_BACK_RIGHT   = 0x20,
        FACE_BOTTOM       = 0x40,
        FACE_TOP          = 0x80,
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

    double get_surface (int mask_ = FACE_ALL) const;

    double get_volume () const;

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

#endif // __geomtools__hexagon_box_h

// end of hexagon_box.h

