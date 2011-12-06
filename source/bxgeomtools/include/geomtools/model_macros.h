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

#define GEOMTOOLS_MODEL_INHERIT 					\
  public geomtools::i_model						\
  /**/

#define GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(ModelClassName)		\
  private:								\
  static geomtools::i_model::creator_registration<ModelClassName> _g_cr_; \
  /**/

#define GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(ModelClassName)		\
  geomtools::i_model::creator_registration<ModelClassName> ModelClassName::_g_cr_; \
  /**/

#endif // __geomtools__model_macros_h

// end of model_macros.h
