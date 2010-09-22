// -*- mode: c++ ; -*- 
/* material.h
 * Author (s) :  Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2010-09-22
 * 
 * License: 
 * 
 * Description: 
 *   A simple class to handle "abstract" material and their basic properties
 * 
 * History: 
 * 
 */

#ifndef __materials__material_h
#define __materials__material_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/units.h>

#include <materials/element.h>

namespace mat {

  using namespace std;
  using namespace datatools::utils;

  class material;

  struct composite_entry
  {
    bool                owned;
    const     element*  iso_ptr;
    const     material* mat_ptr;
    size_t              number_of_atom;
    double              fraction_mass;
    
    composite_entry ();
    ~composite_entry ();
    bool is_valid () const;
  }; 

  typedef std::map<string, composite_entry> composite_map_t; 

  class material : public datatools::utils::i_tree_dumpable
  {
  public:
    static const double g_per_cm3;

    enum composition_t
      {
	SIMPLE_ELEMENT     = 0,
	BY_NUMBER_OF_ATOMS = 1,
	BY_FRACTION_MASS   = 2
      };

  private: 

    string     __name;            //!< Name
    double     __density;         //!< Density 
    // composition:
    int             __mode; //!< mode = "simple_element", "by_number_of_atoms", "by_fraction_mass"
    composite_map_t __composition;

    properties __properties;      //!< datatools properties
    bool       __locked;          //!< boolean flags : true when composition is validated & derived properties are computed

  public: 

    const string & get_name () const;

    void set_name (const string &);

    double get_density () const;

    void set_density (double);
  
  public: 

    // ctor:
    material ();

    // dtor:
    virtual ~material ();
     
    bool is_locked () const {return __locked;} //!<  Return true if composition is valid, weights are normalized and molar mass is computed.       

    const properties & grab_properties () const {return __properties;} //!< Get reference of datatools::properties private attribute

    properties & grab_properties () {return __properties;} //!< Get reference of datatools::properties private attribute
   
    virtual void tree_dump (ostream & out_  = clog,  
			    const string & title_  = "",  
			    const string & indent_ = "",  
			    bool inherit_ = false) const;  //!<  print info virtual method

  };

} // end of namespace mat

#endif // __materials__material_h

// end of material.h
