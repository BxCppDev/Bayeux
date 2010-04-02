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
 - A simple class to handle chemical elements and their basic properties.        
 - Elements are made of isotopes with same number of protons (Z).    
   
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

  typedef std::map<isotope*, double>  map_isotope_weight;  

  class element : public datatools::utils::i_tree_dumpable
  {

  public:   

// ctor/dtor:

    element ();                          //!< Defaut Constructor 
    
    //! Normal Constructor. 
    /*!
 	\param name_ : Name of the element 
     */ 
    element (const  string name_); 
    
      //! Normal Constructor. 
     /*!
 	\param name_                        : Name of the element 
 	\param map_isotope_weight_  : Isotopic compostion of the element map<isotope*, double weight>
      */ 
   // element (const  string name_,  map_isotope_weight  & map_isotope_weight_);

    virtual ~element ();                          //!<  Destructor
 
    private : 					  	  
       
       string               __name;               //!<  Name
       
       string             __symbol;               //!<  Chemical symbol of the element 
       int                  __Z;                       //!<  Number of protons of the element   
       
       double   __molar_mass;                //!<  Molar mass in gramm [g]   
       
   
       
       properties __properties;                 //!<  datatools properties

       // private attributes - boolean flags     
      
       bool   __locked;                            //!<  true if composition is not null & consistant & if molar mass have been computed
       
    //  private set & fin methods :    
    
        void   __set_molar_mass(const double molar_mass_);      //!< Set the molar mass in gramm [g]                          
        
	void   __lock ();              
        void   __unlock ();    
	void   __norm_weights ();                                               //!<  Normalize the sum of weights to unity 	                
        void   __compute_molar_mass ();                                   //!<  Compute molar mass     
	
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
    
      private:  //  critical 
      
   //    iso_entry (const iso_entry &);
    //   iso_entry & operator= (const iso_entry &);
    
      }; 
        
	void   add_isotope (const isotope & iso_ref_, double weight_, bool owned_ = false);        //!<  Add an existed isotope to the composition with weight  
        void   add_isotope (const isotope * iso_ptr_, double weight_);    //!<  Add an non-existed isotope to the composition with weight
  
/*     

     element::iso_entry & element::iso_entry::operator= (const element::iso_entry & entry_)
        {
          owned = false;
          iso_ptr = entry_.iso_ptr;
          weight = entry_.weight;
          return *this;
         }

       element::iso_entry::iso_entry (const iso_entry & e_)
        {
         clog << "DEVEL: element::entry::entry: Copy ctor !" << endl;
         owned = false;
          t = e_.t;
          w = e_.w;
         }
*/ 

   typedef std::map<string,  iso_entry>   isotope_weight_map_t;
   
       isotope_weight_map_t   __composition;	                         //!<  Isotopic composition of the element map<isotope*, double weight> 
   //  public set methods : 
      
      void   set_name(const string  & name_);                                                                    //!<  Set the name  

   //   void   set_composition(map_isotope_weight  & composition_);                                     //!<   Set the composition    vector of pair<isotope*, double weight>      


    // public retrieval methods
      
      const string    get_name()      const {return __name;}        //!<  Return the name   
      const string    get_symbol()    const {return __symbol;}      //!<  Return the chemical symbol   
      
      const double     get_molar_mass()     const {return  __molar_mass;}    //!<  Return the molar mass in gramm/mol [g/mol]   
      const isotope_weight_map_t   get_composition()    const {return __composition;}   //!< Return the composition vector std::map<isotope*, double>      
      
      const bool            is_locked ()     const{return __locked;}      
      
      properties &         grab_properties ()           {return __properties;};
  
   // public tree_dump    

     virtual void tree_dump (ostream & out_         = clog, 
	               		       const string & title_  = "", 
			               const string & indent_ = "", 
			               bool inherit_          = false) const;  

  }; // end of class element

} // end of namespace mat

#endif // __materials__element_h

// end of element.h
