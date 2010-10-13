// -*- mode: c++ ; -*- 
/* i_stackable.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-04-02
 * 
 * License: 
 * 
 * Description: 
 *
 *   Geometry model with multiple stacked boxes.
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_stackable_h
#define __geomtools__i_stackable_h 1

#include <datatools/utils/utils.h>

namespace geomtools {

  class i_stackable
  {
  public:
    
    virtual double get_x () const = 0;
    virtual double get_y () const = 0;
    virtual double get_z () const = 0;
 
  };

  class stackable_data : public i_stackable
  {
  public:

    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;

    stackable_data ()
      {
	datatools::utils::invalidate (xmin);
	datatools::utils::invalidate (xmax);
	datatools::utils::invalidate (ymin);
	datatools::utils::invalidate (ymax);
	datatools::utils::invalidate (zmin);
	datatools::utils::invalidate (zmax);
      }
    double get_x () const {return xmax - xmin;}
    double get_y () const {return zmin - ymin;}
    double get_z () const {return zmax - zmin;}
    
  };

} // end of namespace geomtools

#endif // __geomtools__i_stackable_h

// end of i_stackable.h
