/// \file geomtools/i_boxed_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 *
 * License:
 *
 * Description:
 *
 */

#ifndef GEOMTOOLS_I_BOXED_MODEL_H
#define GEOMTOOLS_I_BOXED_MODEL_H 1

// This project:
#include <geomtools/i_model.h>
#include <geomtools/model_macros.h>

namespace geomtools {

  class box;

  // define a geometry model with a box solid:
  GEOMTOOLS_MODEL_CLASS_DECLARE (i_boxed_model)
  {
  public:

    virtual const geomtools::box & get_box() const = 0;

    i_boxed_model(const std::string & dummy_ = "");

    ~i_boxed_model() override;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_BOXED_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
