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

#ifndef GEOMTOOLS_DETAIL_MODEL_TOOLS_H
#define GEOMTOOLS_DETAIL_MODEL_TOOLS_H

// Standard library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

namespace geomtools {

  // Forward declaration:
  class i_model;

  /// Type alias for dictionary of geometry model plain handles
  typedef std::map<std::string, i_model *> models_col_type;

  typedef i_model * (*model_creator_type) (const std::string & name_,
                                           const datatools::properties & configuration_,
                                           models_col_type * models_);

  typedef std::map<std::string, model_creator_type> model_creator_dict_type;

} // end of namespace geomtools

#endif // GEOMTOOLS_DETAIL_MODEL_TOOLS_H
