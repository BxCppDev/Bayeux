// -*- mode: c++ ; -*- 
/* circle.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 * 
 * License: 
 * 
 * Description: 
 *   A circle in x-y plane
 * 
 * History: 
 * 
 */

#ifndef __geomtools__circle_h
#define __geomtools__circle_h 1

#include <iostream>
#include <string>

#include <geomtools/i_shape_1d.h>

namespace geomtools {

  //using  namespace std;

  class circle : public i_shape_1d
  {
    
  public:
    static const string CIRCLE_LABEL;

  public: 

    bool is_valid () const;

    double get_r () const;

    double get_radius () const;

    void set_r (double a_radius);

    void set_diameter (double a_diameter);
 
    double get_diameter () const;
 
    double get_surface () const;

    double get_circumference () const;

  public: 
    // ctor:
    circle ();

    circle (double a_radius);

    // dtor:
    virtual ~circle ();
  
    // methods:
    virtual string get_shape_name () const;

    virtual void tree_dump (ostream & a_out = clog, 
                            const string & a_title = "", 
                            const string & a_indent = "", 
                            bool a_inherit= false) const;

    virtual bool is_on_curve (const vector_3d &, 
                              double a_tolerance = i_object_3d::USING_PROPER_TOLERANCE) const;

    virtual vector_3d get_direction_on_curve (const vector_3d & a_tposition) const;
    
  private: 

    double _radius_; //!< The radius of the circle (in arbitrary units).
  
  };

} // end of namespace geomtools

#endif // __geomtools__circle_h

// end of circle.h
