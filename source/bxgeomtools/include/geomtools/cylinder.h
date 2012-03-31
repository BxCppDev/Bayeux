// -*- mode: c++; -*- 
/* cylinder.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-24
 * Last modified: 2008-05-24
 * 
 * License: 
 * 
 * Description: 
 *  A cylinder on z-axis
 *
 * History: 
 * 
 */

#ifndef __geomtools__cylinder_h
#define __geomtools__cylinder_h 1

#include <sstream>
#include <stdexcept>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  using namespace std;

  class cylinder : public i_shape_3d , public i_stackable
    {

    public:
      static const std::string CYLINDER_LABEL;

      enum faces_mask_type
        {
          FACE_NONE   = FACE_NONE_BIT,
          FACE_SIDE   = 0x1,
          FACE_BOTTOM = 0x2,
          FACE_TOP    = 0x4,
          FACE_ALL    = (FACE_SIDE
                         | FACE_BOTTOM 
                         | FACE_TOP)
        };  
  
    public: 
    
      double get_xmin () const;
      
      double get_xmax () const;
      
      double get_ymin () const;
      
      double get_ymax () const;
      
      double get_zmin () const;
      
      double get_zmax () const;
      
      double get_z () const;

      void set_z (double);

      void set_diameter (double);

      double get_half_z () const;

      void set_half_z (double);

      double get_r () const;

      double get_radius () const;

      double get_diameter () const;

      void set_r (double);

      void set (double, double);

      void set_alternative (double, double);

    public: 

      // ctor:
      cylinder ();

      // ctor:
      cylinder (double a_radius, double a_z);

      // dtor:
      virtual ~cylinder ();
  
      // methods:
      virtual std::string get_shape_name () const;

      virtual double get_parameter (const std::string &) const;

      bool is_valid () const;

      void init ();

      void reset ();

      double get_surface (int a_mask = FACE_ALL) const;

      double get_volume () const;

      virtual bool is_inside (const vector_3d &, 
                             double a_skin = USING_PROPER_SKIN) const;

      virtual bool is_on_surface (const vector_3d &, 
                                 int a_mask    = FACE_ALL, 
                                 double a_skin = USING_PROPER_SKIN) const;

      virtual vector_3d get_normal_on_surface (const vector_3d & a_position) const;

      virtual bool find_intercept (const vector_3d & a_from, 
                                   const vector_3d & a_direction,
                                   intercept_t & a_intercept,
                                   double a_skin = USING_PROPER_SKIN) const;

      friend std::ostream & operator<< (std::ostream &, const cylinder &);

      friend std::istream & operator>> (std::istream &, cylinder &);

      virtual void tree_dump (std::ostream & a_out         = std::clog, 
                              const std::string & a_title  = "", 
                              const std::string & a_indent = "", 
                              bool a_inherit          = false) const;
       

    private: 

      double _z_;      //<! Height
      double _radius_; //<! Radius

    };
    
} // end of namespace geomtools

#endif // __geomtools__cylinder_h

// end of cylinder.h

