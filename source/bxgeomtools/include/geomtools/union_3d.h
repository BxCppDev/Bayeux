// -*- mode: c++; -*- 
/* union_3d.h
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

#ifndef __geomtools__union_3d_h
#define __geomtools__union_3d_h 1

#include <geomtools/i_composite_shape_3d.h>

namespace geomtools {
  
  class union_3d : public i_composite_shape_3d 
  {
  public:
    static const std::string UNION_3D_LABEL;
    static bool g_devel;
    
  public:

    std::string get_shape_name () const;

    union_3d ();

    virtual ~union_3d ();

    virtual bool is_inside (const vector_3d & position_, 
			    double skin_ = USING_PROPER_SKIN) const;
    
    virtual bool is_on_surface (const vector_3d & position_, 
				int mask_    = ALL_SURFACES, 
				double skin_ = USING_PROPER_SKIN) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;
    
    virtual  bool find_intercept (const vector_3d & from_, 
				  const vector_3d & direction_,
				  intercept_t & intercept_,
				  double skin_ = USING_PROPER_SKIN) const;
    
  };
  
} // end of namespace geomtools

#endif // __geomtools__union_3d_h

// end of union_3d.h
