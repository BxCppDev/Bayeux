// -*- mode: c++ ; -*- 
/* i_wires_3d_rendering.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-10-22
 * Last modified: 2010-10-22
 * 
 * License: 
 * 
 * Description: 
 *   A abstract interface for wires 3D rendering
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_wires_3d_rendering_h
#define __geomtools__i_wires_3d_rendering_h 1

#include <list>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  struct i_wires_3d_rendering
  {
  public:

    virtual ~i_wires_3d_rendering ()
    {
      return;
    }

    virtual void generate_wires (std::list<polyline_3d> & wires_, 
                                 const placement & positioning_, 
                                 uint32_t options_) const = 0;
    
  };

} // end of namespace geomtools

#endif // __geomtools__i_wires_3d_rendering_h

// end of i_wires_3d_rendering.h
