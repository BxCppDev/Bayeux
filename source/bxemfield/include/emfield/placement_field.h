// -*- mode: c++ ; -*- 
/* placement_field.h
 * Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-27
 * Last modified: 2013-02-24
 * 
 * License: 
 * 
 * Description: 
 *
 *   Placement field
 * 
 * History: 
 * 
 */

#include <emfield/base_electromagnetic_field.h>
#include <geomtools/placement.h>

#ifndef EMFIELD_PLACEMENT_FIELD_H_
#define EMFIELD_PLACEMENT_FIELD_H_ 1

namespace emfield {
 
  /** Class representing an uniform (in space) and constant (in time) 
   *  electric field
   */
  EMFIELD_CLASS_DECLARE(placement_field) 
  {
  public:

    EMFIELD_INTERFACE_CTOR_DTOR(placement_field);
    
    const geomtools::placement & get_placement () const;

    void set_placement (const geomtools::placement & p_);

    void set_field (base_electromagnetic_field::handle_type &);
 
  private:
    
    base_electromagnetic_field::handle_type _field_;
    geomtools::placement _placement_;
    
    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(placement_field);
    
  };

} // end of namespace emfield

#endif // EMFIELD_PLACEMENT_FIELD_H_

// end of placement_field.h
