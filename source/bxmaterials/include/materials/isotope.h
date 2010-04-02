// -*- mode: c++ ; -*- 
/* isotope.h
 * Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-03-06
 * Last modified: 2010-03-06
 *
 * A simple class to handle known isotopes (ZAI) and their basic properties. 
 *
 */

/*!
- A simple class to handle known isotopes (ZAI) and their basic properties. 
       
- Isotope are constructed with Z, A and I attributes, used in fact as its identifier.  
	      - Z :  number of protons    or	- "symbol" : chemical symbol 
	      - A :  number of nucleons
	      - I  :  Isomeric state for long half-life time excited states (I=0,1,2,3 = ' ','M','N','O')   
              (Note : for consistency this 3 identifiers cannot be initialize separately)    

- When istope are constructed or after the use of set_ZAI(), a name (in endf6 format) is 
  automatically build  and the mass of isotope is searched on database.   

- Isotope could also have decay data ( find_decay () )  

              - stability flag : true if stable, false if not 
              - decay half life time (+- err) in second [s]    ( =(0.,0.)  if stable )   
 
- Isotope have 2 boolean flags :  
 
             - locked                : true if all identifiers (Z, A, I) are consistant & if mass have been found in database.
	     - has_decay_data : true if decay data has been searched & found in data base.  
   
  \author BG
  @version 0.1
*/


#ifndef __materials__isotope_h
#define __materials__isotope_h 1

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace mat {
  
  using namespace std;
  using namespace datatools::utils;
  
  class isotope : public datatools::utils::i_tree_dumpable
  {   

    public:   /* constructor / destructor */    
      
      isotope();  //!< Defaut Constructor 
      
      //! Normal Constructor. 
      /*!
 	Default: ground state (I=0)
 	\param Z_ : Number of protons  ( 0 <  Z  <= 119)
 	\param A_ : Number of nucleons (Z <= A  <= 293) 
 	\param I_  : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }
      */  
      isotope (const int Z_ , const int A_ , const int I_ =0);               
      
      //! Normal Constructor. 
      /*!
 	Default: ground state (I=0)
 	\param symbol_ : Chemical symbol ( "H " <= symbol  <= "Ei")
 	\param A_         : Number of nucleons  (Z <= A  <= 293) 
 	\param I_          : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }
      */  
      isotope (const string & symbol_ , const int A_  ,  const int I_ =0);        
      
      virtual ~isotope ();                   //!<  Destructor

    private : 					  
       
   // private attributes - Identifiers :      
       
        int               __Z;                 //!<  Number of protons   ( 0  <  Z  <= 119)  
        int               __A;                 //!<  Number of nucleons  (Z <= A  <= 293) 
        int                __I;                 //!<  Isomeric states I={0,1,2,3} ={' ','M','N','O'} for "long half-life" time excited states		  
        string         __symbol;          //!<  Chemical symbol 
       
        string         __name;            //!<  Name in endf6 format "ZZZ-Ch-AAAI" automatically build  
            
   // private attributes - basic properties :       
         
      double                   __mass;                      //!<  Mass of isotope in atomic mass unit   [a.m.u.]
      double             __err_mass;                       //!<  Error on mass in atomic mass unit     [a.m.u.]    
      
      bool                  __is_stable;                      //!<  Stability boolean : true if stable, false if not 
      double                     __rtyp;                      //!<  Decay mode [endf6 format]
      double        __half_life_time;                     //!<  Half life time in second [s]
      double   __err_half_life_time;                     //!<  Error on half life time in second [s]
      
      properties __properties;                             //!<  datatools properties
    
    // private attributes - boolean flags :       
      
       bool   __locked; 	                              //!<  true if all identifiers are consistant  & if mass have been found in database
       bool   __has_decay_data; 	     
      
    //  private set & fin methods : 
        
	void   __set_Z (const int Z_);           //!<  Set Z, the number of protons  : 0 <  Z  <= 119 
	void   __set_A (const int A_);           //!<  Set A,the number of nucleons : Z <= A  <= 293
	void   __set_I (const int I_);             //!<  Set I, the Isomeric state :  I={0,1,2,3} ={' ','M','N','O'}     	
	
        void   __find_Z ();                 //!<  Find & set Z (number of protons) from chemical symbol
        void   __find_symbol ();         //!<  Find & set the chemical symbol from Z (number of protons)
        void   __build_name ();         //!<  Build name : endf6 format "ZZZ-Ch-AAAI"  / example : (Z,A,I) = (53,120,1) => " 53-I -120M"      
        
	
	void   __find_mass ();                                                                            //!<  Search the mass in database 'mass.mass03'
        void   __set_mass (const double mass_, const double err_mass_=0.);       //!<  Set mass and its error in atomic mass unit [a.m.u.]       
     
        void   __set_stable (const bool is_stable_);                                                                                //!<  Set stability flag : true if stable, false if not  
        void   __set_half_life_time (const double half_life_time_ , const double err_half_life_time_ = 0.);    //!<  Set half-life time and its error in second [s]      
                                                    
        void   __lock ();              
        void   __unlock ();                                                                                        
      
   public: 

//  public set methods :    

       void   set_ZAI (const  int Z_, const int A_, const int I_ = 0);              

//  public find methods : 	 
                            
       bool   find_decay ();                                                           //!<  Search decay data in database 'JEFF3.1'          

// public retrieval methods
    
       const int              get_Z ()            const {return __Z;}           //!<  Return Z (number of protons)
       const int              get_A ()            const {return __A;}          //!<  Return A (number of nucleons)
       const int              get_I ()              const {return __I;}           //!<  Return I  (isomeric state)
    
      const string          get_name ()      const {return __name;}           //!<  Return name           
      const string          get_symbol ()   const {return __symbol;}          //!<  Return chemical symbol
      
      const double        get_mass ()         const {return __mass;}            //!<  Return mass in atomic mass unit [a.m.u.]
      const double        get_err_mass ()   const {return __err_mass;}       //!<  Return error on mass in atomic mass unit [a.m.u.]
      
      const bool            is_stable ()                     const {return __is_stable;}               //!<   Return true if isotope is stable
      const double        get_half_life_time ()         const {return __half_life_time;}        //!<   Return half-life time in second [s]
      const double        get_err_half_life_time ()   const {return __err_half_life_time;}   //!<   Return erro on half-life time in second [s]
      
      const bool operator==(isotope &isotope_);	     //!< return true if Z, A & I are the same

      const bool            is_locked ()                    const{return __locked;}   
      const bool            has_decay_data ()         const{return __has_decay_data;}    
      
      properties &         grab_properties ()           {return __properties;};

// public tree_dump  

     virtual void tree_dump (ostream & out_         = clog, 
	               		       const string & title_  = "", 
			               const string & indent_ = "", 
			               bool inherit_          = false) const;  


  }; // end of class isotope

} // end of namespace mat

#endif // __materials__isotope_h

// end of isotope.h
