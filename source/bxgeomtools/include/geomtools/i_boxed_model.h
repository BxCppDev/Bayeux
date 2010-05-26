// -*- mode: c++ ; -*- 
/* i_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 * 
 * License: 
 * 
 * Description: 
 *   
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_boxed_model_h
#define __geomtools__i_boxed_model_h 1

#include <geomtools/i_model.h>
#include <geomtools/box.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a box solid: 
  class i_boxed_model : public i_model 
  {
  public:
    virtual const geomtools::box & get_box () const = 0;
    i_boxed_model (const string & name_ = "") : i_model (name_)
    {
    }
  };

} // end of namespace geomtools

#endif // __geomtools__i_boxed_model_h

// end of i_boxed_model.h

