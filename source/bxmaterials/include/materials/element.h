// -*- mode: c++ ; -*- 
/* element.h
 * Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2010-03-14
 * 
 * Description:  A simple class to handle "abstract" chemical elements and their basic properties.  
 */
 
/*!
  - A simple class to handle "abstract" chemical elements and their basic properties.        
  - Elements is simply a collection of isotopes (weighted by proportion) with same number 
  of protons (Z).  
  - They are construct and validate when invoking build() function.
  - When they are build,  derived properties such as molar mass are computed.  
  \author BG
  @version 0.1
*/
#ifndef __materials__element_h
#define __materials__element_h 1

#include <vector>
#include <map>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <materials/isotope.h>
#include <materials/ch_symbol.h>

namespace mat {

  using namespace std;
  using namespace datatools::utils;
  
  struct iso_entry
  {
    bool               owned;
    const     isotope* iso_ptr;
    double             weight;
    
    iso_entry ();
    ~iso_entry ();
    bool is_valid () const;
  }; 

  typedef std::map<string, iso_entry>   isotope_weight_map_t; 

  class element : public datatools::utils::i_tree_dumpable
  {
  public:

    static const int Z_UNDEFINED = -1;

  protected:

    void _lock_check (const string & where_) const;

  public:  

    /* constructor / destructor */   
    element ();          //!< Defaut Constructor 
    
    //! Normal Constructor. 
    /*!
      \param name_ : Name of the element
      \param Z_ : Z of the element 
      \param build_ : On the fly building 
    */ 
    element (const string & name_, int Z_, bool build_ = false);   
    
    //! Normal Constructor. 
    /*!
      \param name_ : Name of the element
      \param Z_ : Z of the element 
      \param A_ : Molar mass of the element 
      \param build_ : On the fly building 
    */ 
    element (const string & name_, int Z_, double molar_mass_, bool build_ = false);   

    //! Normal Constructor. 
    /*!
      \param name_ : Name of the element
      \param symbol_ : Chemical symbol of the element 
      \param build_ : On the fly building 
    */ 
    element (const  string & name_, const string & symbol_ = "", bool build_ = false); 

    virtual ~element ();          //!<  Destructor
   
    /* private attribute */    
  private :   
    
    string     __name;            //!<  Name
    string     __symbol;          //!<  Chemical symbol of the element 
    int        __Z;               //!<  Number of protons of the element   
    isotope_weight_map_t   __composition;  //!<  Isotopic composition of the element [std::map<string,  iso_entry>]
    double     __molar_mass;      //!<  Molar mass in [g/mol]     
    properties __properties;      //!<  datatools properties
    bool       __locked;          //!<  boolean flag : true when composition is validated & derived properties are computed
        
    /* private set & find methods */    
  private :       
     
    //    void   __init ();               //!<  Initialize or reinitialize  Z, symbol, composition, molar mass and lock attribute   
    void   __norm_weights ();       //!<  Normalize sum of weights to unity 	                     
    void   __compute_molar_mass (); //!<  Compute molar mass in[g/mol]   
    void   __set_molar_mass(const double molar_mass_);  //!< Set the molar mass in [g/mol]                                  
    void   __lock ();   //!<  Lock the element : boolean flag '__locked' is set to true when composition is valid & derived properties are computed.              
    void   __unlock (); //!<  Unlock the element - boolean flag '__locked' is set to false.       	 
				     					  	 
    /* public methods */ 
  public :
    bool   is_built_by_isotopic_composition () const;
    void   set_name (const string  & name_); //!<  Set the name of the element           
    void   set_Z (const int  Z_);            //!<  Set the Z (number of protons) & the chemical symbol of the element.      
    void   set_Z (const string  & symbol_);  //!<  Set the Z (number of protons) & the chemical symbol of the element.  
    void   set_molar_mass(const double molar_mass_); //!<  Set the molar mass of the element.  
    void   add_isotope (const isotope & iso_ref_ , double weight_ = 1.0, bool owned_ = false);  //!<  Add an isotope with weight          
    void   add_isotope (const isotope * iso_ptr_ , double weight_ = 1.0);   //!<  Add an isotope with weight                                        
    void   build ();            //!<  Build the element : norm weights, compute molar mass and lock (or not).                
    void   build_from_nist ();  //!<  Build the element using nist composition data, then normalize weights, compute molar mass and lock (or not).  
                                              
 
    /* public retrieval methods */
  public : 
      
    const string   get_name () const {return __name;}              //!<  Return the name.       
    const string   get_symbol () const {return __symbol;}          //!<  Return the chemical symbol. 
    const int      get_Z () const {return __Z;}                    //!<  Return the number of protons (Z).             
    const double   get_molar_mass () const {return  __molar_mass;} //!<  Return the molar mass [g/mol].           
    const bool     is_locked () const {return __locked;}           //!<  Return true if composition is valid, weights are normalized and molar mass is computed.       
    const isotope_weight_map_t & get_composition () const  
    {
      return __composition;
    }

    const properties & grab_properties () const {return __properties;} //!< Get reference of datatools::properties private attribute

    properties &   grab_properties ()     {return __properties;}     //!<Get reference of datatools::properties private attribute
   
    virtual void tree_dump (ostream & out_  = clog,  const string & title_  = "",  const string & indent_ = "",  bool inherit_ = false) const;  //!<  print info virtual method


  }; // end of class element

} // end of namespace mat

#endif // __materials__element_h

// end of element.h
