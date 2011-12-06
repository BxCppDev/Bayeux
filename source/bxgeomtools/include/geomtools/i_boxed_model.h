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
#include <geomtools/model_macros.h>

namespace geomtools {

  using namespace std;

  class box;

  // define a geometry model with a box solid: 
  class i_boxed_model : GEOMTOOLS_MODEL_INHERIT 
  {
  public:

    virtual const geomtools::box & get_box () const = 0;

    i_boxed_model (const string & a_name = "");

    virtual ~i_boxed_model ();

  };

} // end of namespace geomtools

#define GEOMTOOLS_BOXED_MODEL_INHERIT 					\
  public geomtools::i_boxed_model					\
  /**/

#endif // __geomtools__i_boxed_model_h

// end of i_boxed_model.h

