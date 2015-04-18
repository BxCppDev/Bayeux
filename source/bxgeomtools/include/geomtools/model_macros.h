/// \file geomtools/model_macros.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-12-05
 * Last modified: 2011-12-05
 *
 * License:
 *
 * Description:
 *
 *   Macros for geometry models
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MODEL_MACROS_H
#define GEOMTOOLS_MODEL_MACROS_H 1

// This project:
#include <geomtools/i_model.h>

#define GEOMTOOLS_MODEL_INHERIT                 \
  public ::geomtools::i_model                   \
  /**/

#define GEOMTOOLS_MODEL_CLASS_DECLARE(ModelClassName)   \
  class ModelClassName : GEOMTOOLS_MODEL_INHERIT        \
  /**/

#define GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(ModelClassName)          \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::geomtools::i_model,ModelClassName); \
  /**/

#define GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(ModelClassName,ModelClassId) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::geomtools::i_model,ModelClassName,ModelClassId); \
  /**/

#define GEOMTOOLS_MODEL_PRE_CONSTRUCT_INTERFACE()               \
  protected:                                                    \
  virtual void _pre_construct (::datatools::properties &);      \
  /**/

#define GEOMTOOLS_MODEL_POST_CONSTRUCT_INTERFACE()              \
  protected:                                                    \
  virtual void _post_construct (::datatools::properties &);     \
  /**/

#define GEOMTOOLS_MODEL_AT_CONSTRUCT_INTERFACE()                        \
  protected:                                                            \
  virtual void _at_construct (const std::string &,                      \
                              const ::datatools::properties &,          \
                              ::geomtools::models_col_type * = 0);      \
  /**/

#define GEOMTOOLS_MODEL_AT_CONSTRUCT_IMPL_HEAD(ModelClassName,Name,Setup,Models) \
  void ModelClassName::_at_construct (const std::string & Name,         \
                                      const ::datatools::properties & Setup, \
                                      ::geomtools::models_col_type * Models) \
  /**/

#endif // GEOMTOOLS_MODEL_MACROS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
