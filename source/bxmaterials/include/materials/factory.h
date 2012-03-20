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

#include <string>

// #include <datatools/utils/properties.h>
#include <materials/detail/tools.h>
//#include <materials/isotope.h>
//#include <materials/element.h>
// #include <materials/material.h>

namespace datatools {
  namespace utils {
    class properties;
  }
}

namespace mat {

  class isotope;
  class element;
  class material;

  class factory
  {
  private: 
    bool _debug_;

  public: 
    bool is_debug () const;
    void set_debug (bool);
  
  public: 

    // ctor:
    factory ();

    // dtor:
    virtual ~factory ();
  
    isotope * create_isotope (const std::string & name_, 
                              const datatools::utils::properties & config_) const;
  
    element * create_element (const std::string & name_,
                              const datatools::utils::properties & config_, 
                              const isotope_dict_t & isotopes_) const;
  
    material * create_material (const std::string & name_, 
                                const datatools::utils::properties & config_, 
                                const element_dict_t & elements_, 
                                const material_dict_t & materials_) const;
    

  };

} // end of namespace mat

#endif // __materials__factory_h

// end of factory.h
