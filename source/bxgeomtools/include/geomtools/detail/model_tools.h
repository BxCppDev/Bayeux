// -*- mode: c++ ; -*- 
/* model_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2010-02-12
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __geomtools__detail__model_tools_h
#define __geomtools__detail__model_tools_h 1

#include <string>
#include <map>

#include <datatools/utils/properties.h>

namespace geomtools {

  using namespace std;
  using namespace datatools::utils;

  class i_model;
    
  typedef map <string, i_model *> models_col_t;

  typedef i_model * (*model_creator_t) (const string & name_,
					const properties & configuration_,
					models_col_t * models_);

  typedef map<string, model_creator_t> model_creator_dict_t;

} // end of namespace genvtx

#endif // __geomtools__detail__model_tools_h

// end of model_tools.h
