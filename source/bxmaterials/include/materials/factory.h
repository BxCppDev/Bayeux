// -*- mode: c++ ; -*- 
/* factory.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2010-09-22
 * 
 * License: 
 * 
 * Description: 
 *   A factory for dynamic creation of isotopes/elements/materials
 * 
 * History: 
 * 
 */

#ifndef __materials__factory_h
#define __materials__factory_h 1

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <datatools/utils/properties.h>

#include <materials/detail/tools.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>

namespace mat {

  class factory
  {
  private: 
    bool __debug;

  public: 
    bool is_debug () const;
    void set_debug (bool);
  
  // ctor/dtor:
  public: 

    factory ();

    virtual ~factory ();
  
    isotope * create_isotope (const string & name_, 
			      datatools::utils::properties & config_);
  
    element * create_element (const string & name_,
			      datatools::utils::properties & config_, 
			      const isotope_dict_t & isotopes_);
  
    material * create_material (const string & name_, 
				datatools::utils::properties & config_, 
				const element_dict_t & elements_, 
				const material_dict_t & materials_);
    

  };

} // end of namespace mat

#endif // __materials__factory_h

// end of factory.h
