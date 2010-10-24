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
 *  Note: 
 *   some useful data here:
 *     http://www.nucsafe.com/cms/Selecting+Gamma+Detector/43.html
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

#include <vector>
#include <map>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/units.h>

#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/isotope.h>


namespace mat {
  
  using namespace std;
  using namespace datatools::utils;
  
  class material;
  
  struct compound_entry
  {
    bool             owned;
    const element *  elt_ptr; 
    const material * mat_ptr;
    double           weight; 
    int              nb_of_atoms;
    
    compound_entry ();
    ~compound_entry ();
    bool is_valid () const;
  }; 
  
  typedef std::map<string, compound_entry>   composition_map_t; 
  
  class material : public datatools::utils::i_tree_dumpable
  {
    
    public:
    
    //! Define the proportion units : 
    /*! KP_ATOM (number of atoms by molecule) or KP_MASS (% mass)
     */
   
    enum proportion_unit_type 
      { 
	KP_UNDEFINED = -1, 
	KP_ATOM = 0, 
	NUMBER_OF_ATOMS = 0,
	KP_MASS = 1, 
	FRACTION_MASS = 1,
	KP_ZA = 2, 
	MEAN_ZA = 2
      };
    
      static const double g_per_cm3;

    public: 
    
     /* constructor / destructor */   
     material ();          //!< Defaut Constructor 
    
    //! Normal Constructor. 
    /*!
      \param name_ : Name of the material
    */ 
    material (const string & name_);   

    virtual ~material ();          //!<  Destructor


   private: 

    string     __name;    //!< Name
    double     __density; //!< Density 
    double     __mean_z;  //!< Mean atomic number
    double     __mean_a;  //!< Mean mass number

    // composition:
  
    proportion_unit_type __proportion_unit; //!< KP_ATOM (number of atoms by molecule) or KP_MASS (% mass)
    composition_map_t   __composition;      //!<  composition of the material [std::map<string, compound_entry>]
        
    properties __properties;                //!< datatools properties
    bool       __locked;                    //!< boolean flags : true when composition is validated & derived properties are computed
    void _lock_check (const string & where_) const;
   
    void   __compute_molar_mass ();         //!<  Compute molar mass in[g/mol]   
    void   __lock ();                       //!<  Lock the element : boolean flag '__locked' is set to true     

 
  public: 

    bool is_composed_by_mean_z_a () const;

    bool is_composed_by_number_of_atoms () const;

    bool is_composed_by_fraction_mass () const;

    const composition_map_t &  get_composition () const;   //!<  access to the composition map

    const string & get_name () const;

    void set_name (const string &);

    double get_mean_z () const;

  private:

    void __set_mean_z (double); 

  public:

    double get_mean_a () const;

  private:

    void __set_mean_a (double); 

  public:

    void set_mean_z_a (double z_, double a_); 

    double get_density () const;

    void set_density (double); 
    
    void add_element_by_mass (const element & elt_ref_ , double weight_  , bool owned_ = false);    
    //!<  Add an element with weight in KP_MASS proportion unit          
  
    void add_element_by_mass (const element * elt_ptr_ , double weight_ );                                   
    //!<  Add an element with weight in KP_MASS proportion unit                                        

    void add_element_by_nb_of_atoms (const element & elt_ref_ , int nb_of_atoms_  , bool owned_ = false);    
     //!<  Add an element  in KP_ATOM proportion unit          
  
     void add_element_by_nb_of_atoms (const element * elt_ptr_ , int nb_of_atoms_   );                                   
     //!<  Add an element in KP_ATOM proportion unit             
          
     void add_material_by_mass (const material & mat_ref_ , double weight_  , bool owned_ = false);    
    //!<  Add an material with weight in KP_MASS proportion unit          
  
    void add_material_by_mass (const material * mat_ptr_ , double weight_ );                                   
    //!<  Add an element with weight in KP_MASS proportion unit    
    
    void build (); //!<  Build the material :  compute molar mass and lock (or not).                                                                              
  private:
    
    void __normalize_weights ();    

  public: 

     const bool is_locked () const {return __locked;} //!<  Return true if composition is valid, weights are normalized and molar mass is computed.       

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
