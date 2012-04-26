// -*- mode: c++ ; -*- 
/* uniform_electric_field.h
 * Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-25
 * Last modified: 2012-04-25
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

#ifndef __emfield__uniform_electric_field_h
#define __emfield__uniform_electric_field_h 1

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

#endif // __emfield__uniform_electric_field_h

// end of uniform_electric_field.h
