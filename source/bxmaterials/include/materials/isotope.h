// -*- mode: c++ ; -*- 
/* isotope.h
 * Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-03-06
 * Last modified: 2010-03-06
 *
 * Description : A simple class to handle only isotopes (ZAI) and their basic properties (mass & decays). 
 *
 */
 
#ifndef __materials__isotope_h
#define __materials__isotope_h 1

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>
#include <materials/ch_symbol.h>

namespace mat {
  
  using namespace std;
  using namespace datatools::utils;
  
   /*! \brief A simple class to handle known isotopes (ZAI) and their basic properties (mass & decays). 
     *    
     *  - Isotope are build with a name and (Z, A, I) attributes :  
     * 	       - name : which should be used as its id  ( = 'ChA-(I)' or '?-0' by default ) 
     *	       - Z :  number of protons equivalent to the chemical symbol     ( = 0 by default )     
     * 	       - A :  number of nucleons    ( = 0 by default ) 
     *	       - I  :  Isomeric state if long half-life time excited states { 0 = ground state =' ' ; 1 = 
     *                  first excited ='M' ;  2 =.....='N'  ;  3 = third  excited='O' }  ( = 0 by default ) 
     *
     * 	 - I suppose we will only use known isotopes, thus it 's should be not possible to create an unknow ZAI ( except with default constructor ) : 
     * 	       - For consistency, the 3 attributes (Z,A,I) cannot be initialized separately
     *	       - Tabulated value of (Z,A) are searched in data file ( 'resources/mass.mas03' by default ) 
     * 	       - Note : Isomeric state (I) stay a 'free' parameter which is not checked for the moment
     *	         
     *	Isotope have mass data [mass with its error], which can be initialize with 'set_mass ()' or 'find_mass ()' methods  ( = [0.,0.] by default ).
     *
     *  Likewise, Isotope have decay data [stability flag, half-life time with its error], initialized with 'set_decay ()' or 'find_decay ()' methods (= [unstable, -1, -1] by default).
     *
     *  Note : When the set_ZAI method is invoked, all others attributes (mass & decay) are reinitialized to their default values.
     *     
      \author BG
      @version 1.0	                 
     */
  
  class isotope : public datatools::utils::i_tree_dumpable
  {   

    public:   /* constructor - destructor */    
      
      isotope ();          //!< Default Constructor   
      isotope (const string & name_);          //!< Normal Constructor        
      isotope (const string & name_, const int Z_ , const int A_ , const int I_ =0);          //!< Normal Constructor      
      isotope (const string & name_, const string & ch_symbol_ , const int A_  ,  const int I_ =0);          //!< Normal Constructor         
      virtual ~isotope ();          //!< Destructor

    private :   /* private attributes */  					  
      
      int __Z;          //!<  Number of protons   (0<Z<=119)   
      int __A;          //!<  Number of nucleons  (Z<=A<=293)  
      int __I;           //!<  Isomeric states I={0,1,2,3} ={' ','M','N','O'} for "long half-life" time excited states 		    
      
      string __name;          //!<  Name of the isotope, which should be used as its id ( by default = 'ChA(I)' = ZAI name )    
                  
      double __mass;          //!<  Mass in gramm per mol [g/mol]
      double __err_mass;          //!<  Error on the mass in gramm per mol [g/mol]     
       
      double __half_life_time;          //!<  Half life time in second [s]
      double __err_half_life_time;          //!<  Error on the half life time in second [s]
      
      bool __is_known;          //!<  Boolean flag [false by default] = true is (Z,A) values are founded in file mass.mas03
      
      properties __properties;                             //!<  datatools properties         
    
    private :   /* private set & find methods */  					  
      
      
      void __check_ZA ();           //!<  true is (Z,A) values are founded in file mass.mas03   
      
      void __init ();           //!<  Initialize attributes according to new (Z, A, I) values   
      
      void __set_Z (const int Z_);           //!<  Set Z, the number of protons  : 0 <  Z  <= 119  (check if exist)
      void __set_A (const int A_);           //!<  Set A,the number of nucleons : Z <= A  <= 293
      void __set_I (const int I_);             //!<  Set I, the Isomeric state :  I={0,1,2,3} ={' ','M','N','O'} 

      void __set_name (const string & name_);         //!< Set the name             
   
      void __set_mass (const double mass_, const double err_mass_=0);          //!<  Set the mass and its error in gramm per mol [g/mol]       
      
      void __set_half_life_time (const double half_life_time_ , const double err_half_life_time_);    //!<  Set the half-life time and its error in second [s]     
                                                                                                                                                       
   public:   

      /* public set & find methods */   
      
      void set_name (const  string & name_);          //!<  Set the name
      void set_ZAI (const  int Z_, const int A_, const int I_ = 0);          //!<  Set Z, A, I attributes 
      
      void set_mass (const double mass_, const double err_mass_);          //!<  Set the mass in gramm per mol [g/mol] 
      void find_mass (const  string & input_file_name_ = "mass.mas03");          //!<  Search & set the mass from input file (= 'resources/mass.mass03' by default)
       
      void set_decay (const double half_life_time_, const double err_half_life_time_=0);          //!<  Set the decay data 
      void find_decay (const  string & input_file_name_ = "JEFF311RDD_ALL.OUT");           //!<  Search & set the decay data from input file (= 'resources/JEFF311RDD_ALL.OUT' by default)           
  
      /* const retrieval methods */        
       
      const string get_name ()     const {return __name;}          //!< Return the name 
      const string get_ch_symbol ()     const;          //!< Return the chemical symbol        
      const string get_ZAI_name ()     const;         //!< Return the ZAI name : 'ChA(I)'         
      
      const int get_Z ()     const {return __Z;}          //!< Return Z (number of protons)
      const int get_A ()     const {return __A;}          //!< Return A (number of nucleons)
      const int get_I ()     const {return __I;}          //!< Return I (isomeric state)     
      
      const bool is_known () const ;          //!< Return __is_known boolean flag
      
      const bool has_mass_data () const ;          //!< Return true if mass data have been set properly, false either
      const double get_mass ( )     const {return __mass;}          //!<  Return the mass in gramm per mol [g/mol]
      const double get_err_mass ()     const {return __err_mass;}          //!<  Return the error on the mass in gramm per mol [g/mol]
      
      const bool has_decay_data () const ;          //!< Return true if decay data have been set properly, false either  
      const bool is_stable ()     const;          //!< Return true if isotope is stable, false either 
      const double get_half_life_time ()     const {return __half_life_time;}          //!< Return the half-life time in second [s]
      const double get_err_half_life_time ()     const {return __err_half_life_time;}          //!< Return the error on half-life time in second [s]
      
      const bool is_locked ()     const ;         //!< Return true if isotope is locked (i.e. fully set), false either    
       
       //! Print info      
      
      virtual void tree_dump (ostream & out_         = clog, 
	               		       const string & title_  = "", 
			               const string & indent_ = "", 
			               bool inherit_          = false) const;          
      //! Give acces to properties 
      
      properties & grab_properties ()     {return __properties;  };
                
  }; // end of class isotope

} // end of namespace mat

#endif // __materials__isotope_h

// end of isotope.h
