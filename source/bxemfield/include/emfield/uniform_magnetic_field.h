// -*- mode: c++ ; -*- 
/* uniform_magnetic_field.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <emfield/base_electromagnetic_field.h>

#ifndef EMFIELD_UNIFORM_MAGNETIC_FIELD_H_
#define EMFIELD_UNIFORM_MAGNETIC_FIELD_H_ 1

namespace emfield {
 
  /** Class representing an uniform (in space) and constant (in time) 
   *  magnetic field
   */
  EMFIELD_CLASS_DECLARE(uniform_magnetic_field) 
  {
  public:

    EMFIELD_INTERFACE_CTOR_DTOR(uniform_magnetic_field);
    
    const geomtools::vector_3d & get_uniform_magnetic_field () const;

    void set_uniform_magnetic_field (const geomtools::vector_3d & b_);

  private:
    
    geomtools::vector_3d _uniform_magnetic_field_;
    
    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(uniform_magnetic_field);
    
  };

} // end of namespace emfield

#endif // EMFIELD_UNIFORM_MAGNETIC_FIELD_H_

// end of uniform_magnetic_field.h
