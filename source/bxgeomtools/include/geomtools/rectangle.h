// -*- mode: c++ ; -*- 
/* rectangle.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 * 
 * License: 
 * 
 * Description: 
 *   A rectangle in x-y plane
 * 
 * History: 
 * 
 */

#ifndef __geomtools__rectangle_h
#define __geomtools__rectangle_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <gsl/gsl_poly.h>

#include <geomtools/i_shape_2d.h>

namespace geomtools {

  using namespace std;

  class rectangle : public i_shape_2d
  {
    
  public:
    static const string RECTANGLE_LABEL;
    
  private: 
    double __x;
    double __y;

  public: 

    bool is_valid () const;

    double get_x () const;

    double get_y () const;

    void set_x (double);

    void set_y (double);
 
    double get_surface () const;

    double get_circumference () const;
 
    double get_diagonal () const;
 
 
  public: 
    // ctor:
    rectangle ();

    rectangle (double x_, double y_);

    // dtor:
    virtual ~rectangle ();
  
    // methods:
    virtual string get_shape_name () const;
      
    virtual bool is_on_surface (const vector_3d &,
				double tolerance_ = USING_PROPER_TOLERANCE) const; 
    
    virtual vector_3d get_normal_on_surface (const vector_3d & position_,
					     bool up_ = true) const;
    
    virtual bool find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double tolerance_ = USING_PROPER_TOLERANCE) const;

    virtual void tree_dump (ostream & out_ = clog, 
			    const string & title_ = "", 
			    const string & indent_ = "", 
			    bool inherit_= false) const;
 
  };

} // end of namespace geomtools

#endif // __geomtools__rectangle_h

// end of rectangle.h
