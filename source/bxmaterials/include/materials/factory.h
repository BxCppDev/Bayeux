// -*- mode: c++ ; -*- 
/* factory.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2013-02-23
 * 
 * License: 
 * 
 * Description: 
 *   A factory for dynamic creation of isotopes/elements/materials
 * 
 * History: 
 * 
 */

#ifndef MATERIALS_FACTORY_H_
#define MATERIALS_FACTORY_H_ 1

#include <string>

#include <materials/detail/tools.h>

namespace datatools {
    class properties;
}

namespace materials {

  class isotope;
  class element;
  class material;

  class factory
  {
  public: 

    bool is_debug () const;

    void set_debug (bool);
 
    /// Constructor
    factory ();

    /// Destructor
    virtual ~factory ();
  
    isotope * create_isotope (const std::string & name_, 
                              const datatools::properties & config_) const;
  
    element * create_element (const std::string & name_,
                              const datatools::properties & config_, 
                              const isotope_dict_type & isotopes_) const;
  
    material * create_material (const std::string & name_, 
                                const datatools::properties & config_, 
                                const element_dict_type & elements_, 
                                const material_dict_type & materials_) const;
    
  private: 

    bool _debug_; //!< Debug flag

  };

} // end of namespace materials

#endif // MATERIALS_FACTORY_H_

// end of factory.h
