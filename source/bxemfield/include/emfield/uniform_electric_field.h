/// \file emfield/uniform_electric_field.h
/* Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-25
 * Last modified: 2013-02-24
 *
 * License:
 *
 * Description:
 *
 *   Uniform electric field
 *
 * History:
 *
 */

#ifndef EMFIELD_UNIFORM_ELECTRIC_FIELD_H
#define EMFIELD_UNIFORM_ELECTRIC_FIELD_H 1

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** Class representing an uniform (in space) and constant (in time)
   *  electric field
   */
  EMFIELD_CLASS_DECLARE(uniform_electric_field)
  {
  public:

    EMFIELD_INTERFACE_CTOR_DTOR(uniform_electric_field);

    const geomtools::vector_3d & get_uniform_electric_field () const;

    void set_uniform_electric_field (const geomtools::vector_3d & b_);

  private:

    geomtools::vector_3d _uniform_electric_field_;

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(uniform_electric_field);

  };

} // end of namespace emfield

#endif // EMFIELD_UNIFORM_ELECTRIC_FIELD_H

// Local Variables: --
// mode: c++ --
// End: --
