// -*- mode: c++ ; -*- 
/* uniform_electric_field.h
 * Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <emfield/base_electromagnetic_field.h>

#ifndef EMFIELD_UNIFORM_ELECTRIC_FIELD_H_
#define EMFIELD_UNIFORM_ELECTRIC_FIELD_H_ 1

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

#endif // EMFIELD_UNIFORM_ELECTRIC_FIELD_H_

// end of uniform_electric_field.h
