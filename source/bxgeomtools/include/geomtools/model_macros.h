// -*- mode: c++ ; -*- 
/* model_macros.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-12-05
 * Last modified: 2011-12-05
 * 
 * License: 
 * 
 * Description: 
 *   Macros for geometry models
 * 
 * History: 
 * 
 */

#ifndef __geomtools__model_macros_h
#define __geomtools__model_macros_h 1

#include <geomtools/i_model.h>

#define GEOMTOOLS_MODEL_INHERIT \
  public ::geomtools::i_model   \
  /**/

#define GEOMTOOLS_MODEL_CLASS_DECLARE(ModelClassName)             \
  class ModelClassName : GEOMTOOLS_MODEL_INHERIT                  \
   /**/

#define GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(ModelClassName)          \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::geomtools::i_model,ModelClassName); \
  /**/

#define GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(ModelClassName,ModelClassId)          \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::geomtools::i_model,ModelClassName,ModelClassId); \
  /**/

#endif // __geomtools__model_macros_h

// end of model_macros.h
