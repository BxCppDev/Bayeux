// -*- mode: c++ ; -*- 
/* manager.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2010-09-22
 * 
 * License: 
 * 
 * Description: 
 *   A manager for isotopes/elements and materials
 * 
 * History: 
 * 
 */

#ifndef __materials__manager_h
#define __materials__manager_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <materials/factory.h>
#include <datatools/utils/multi_properties.h>

namespace mat {

  class manager : public datatools::utils::i_tree_dumpable
  {
  private: 

    bool            __debug;
    factory         __creator;
    isotope_dict_t  __isotopes;
    element_dict_t  __elements;
    material_dict_t __materials;
    bool            __locked;

  public: 
    bool is_debug () const;
    void set_debug (bool);
    bool is_locked () const;
    const isotope_dict_t & get_isotopes () const;
    const element_dict_t & get_elements () const;
    const material_dict_t & get_materials () const;
  
  public: 
    // ctor:
    manager ();

    // dtor:
    virtual ~manager ();
  
    void load (const datatools::utils::multi_properties & config_);

    void lock ();

    void unlock ();

    //! Print info      
    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;          

  };

} // end of namespace mat

#endif // __materials__manager_h

// end of manager.h
