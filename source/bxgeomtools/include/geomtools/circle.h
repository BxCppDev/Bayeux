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

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <gsl/gsl_poly.h>

#include <geomtools/i_shape_1d.h>

namespace geomtools {

  using namespace std;

  class circle : public i_shape_1d
  {
    
  public:
    static const string CIRCLE_LABEL;
    
  private: 
    double __r;

  public: 

    bool is_valid () const;

    double get_r () const;

    double get_radius () const;

    void set_r (double);

    void set_diameter (double);
 
    double get_diameter () const;
 
  public: 
    // ctor:
    circle ();

    circle (double r_);

    // dtor:
    virtual ~circle ();
  
    // methods:
    virtual string get_shape_name () const;

    virtual void tree_dump (ostream & out_ = clog, 
			    const string & title_ = "", 
			    const string & indent_ = "", 
			    bool inherit_= false) const;

    virtual bool is_on_curve (const vector_3d &, 
			      double tolerance_ = i_object_3d::USING_PROPER_TOLERANCE) const;

    virtual vector_3d get_direction_on_curve (const vector_3d & position_) const;
  
  };

} // end of namespace geomtools

#endif // __geomtools__circle_h

// end of circle.h
