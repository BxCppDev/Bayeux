/// \file emfield/uniform_magnetic_field.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-24
 * Last modified: 2013-02-24
 *
 * License:
 *
 * Description:
 *
 *   Uniform magnetic field
 *
 * History:
 *
 */

#ifndef EMFIELD_UNIFORM_MAGNETIC_FIELD_H
#define EMFIELD_UNIFORM_MAGNETIC_FIELD_H 1

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** Class representing an uniform (in space) and constant (in time)
   *  magnetic field
   */
  class uniform_magnetic_field : public base_electromagnetic_field
  {
  public:

    EMFIELD_INTERFACE_CTOR_DTOR(uniform_magnetic_field);

    const geomtools::vector_3d & get_uniform_magnetic_field() const;

    void set_uniform_magnetic_field(const geomtools::vector_3d & b_);

  private:

    geomtools::vector_3d _uniform_magnetic_field_; //!< The uniform magnetic field

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(uniform_magnetic_field);

  };

} // end of namespace emfield

#endif // EMFIELD_UNIFORM_MAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
