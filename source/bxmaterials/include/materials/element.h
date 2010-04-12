// -*- mode: c++ ; -*- 
/* element.h
 * Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2010-03-14
 * 
 * Description:  A simple class to handle "abstract" chemical elements and their basic properties. 
 * 
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

namespace mat {

  using namespace std;
  using namespace datatools::utils;

  class element : public datatools::utils::i_tree_dumpable
  {

  public:   /* constructor / destructor */    

    element ();                          //!< Defaut Constructor 
    
    //! Normal Constructor. 
    /*!
 	\param name_ : Name of the element 
     */ 
    element (const  string name_); 

    virtual ~element ();                          //!<  Destructor
 
    private : 					  	  
       
       string               __name;               //!<  Name
       
       string             __symbol;               //!<  Chemical symbol of the element 
       int                  __Z;                       //!<  Number of protons of the element   
       
       double   __molar_mass;                //!<  Molar mass in gramm [g]     
       
       properties __properties;                 //!<  datatools properties

       // private attributes - boolean flags     
      
       bool   __locked;                            //!<  true when composition is validated & derived properties are computed
	
   public :

// public structure iso_entry :          

     struct iso_entry
       {
         bool                   owned;
         const     isotope* iso_ptr;
         double               weight;
    
          iso_entry ();
        ~iso_entry ();
          bool is_valid () const;
       }; 

     typedef std::map<string,  iso_entry>   isotope_weight_map_t; 
     
     isotope_weight_map_t   __composition;	                    //!<  Isotopic composition of the element 


//  public set & add methods : 
    
     void   set_name (const string  & name_);      //!<  Set the name of the element    
    
     /*!
 	Add an existed isotope to the composition with weight 
      */      
     void   add_isotope (const isotope & iso_ref_ , double weight_  , bool owned_ = false);  
    
      /*!
  	Add an non-existed isotope iso_ptr_ to the composition with weight 
      */   
     void   add_isotope (const isotope *  iso_ptr_ , double weight_);                                  
  
     void  build ();               
 
 
 
//  private set & find methods :    
    
    private : 
        
	void   __norm_weights ();                          //!<  Normalize weights to unity 	                
        void   __compute_molar_mass ();               //!<  Compute molar mass     
        void   __set_molar_mass(const double molar_mass_);      //!< Set the molar mass in gramm [g]                          
        
	void   __lock ();              
        void   __unlock ();    	

// public retrieval methods
    
      public : 
      
        const string    get_name ()              const {return __name;}              //!<  Return the name   
        const string    get_symbol ()            const {return __symbol;}            //!<  Return the chemical symbol   
      
        const double  get_molar_mass ()     const {return  __molar_mass;}    //!<  Return the molar mass in gramm/mol [g/mol]      
      
         const bool      is_locked ()     const{return __locked;}      
      
         properties &   grab_properties ()           {return __properties;};
   

        virtual void tree_dump (ostream & out_         = clog,  const string & title_  = "", 
			                 const string & indent_ = "",  bool inherit_          = false) const;  

  }; // end of class element

} // end of namespace mat

#endif // __materials__element_h

// end of element.h
