// -*- mode: c++ ; -*- 
/* disk.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 * 
 * License: 
 * 
 * Description: 
 *   A disk in x-y plane
 * 
 * History: 
 * 
 */

#ifndef __geomtools__disk_h
#define __geomtools__disk_h 1

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

  class disk : public i_shape_2d
  {
    
  public:
    static const string DISK_LABEL;
    
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
    disk ();

    disk (double r_);

    // dtor:
    virtual ~disk ();
  
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

  };

} // end of namespace geomtools

#endif // __geomtools__disk_h

// end of disk.h
