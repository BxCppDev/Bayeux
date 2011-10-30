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
   *  Note : When the set_zai method is invoked, all others attributes (mass & decay) are reinitialized to their default values.
   *     
   \author BG
   @version 1.0	                 
  */
  
  class isotope : public datatools::utils::i_tree_dumpable
  {   
  public:

    enum isomeric_level_t
      {
	GROUND_STATE   = 0,
	FIRST_EXCITED  = 1,
	ISOMERIC_M     = 1,
	SECOND_EXCITED = 2,
	ISOMERIC_N     = 2,
	THIRD_EXCITED  = 3,
	ISOMERIC_O     = 3
      };

  public:   /* constructor - destructor */    
      
    isotope ();                      //!< Default Constructor   
    isotope (const string & name_);  //!< Normal Constructor        
    isotope (const string & name_, 
	     const int z_ , 
	     const int a_ , 
	     const int i_ = GROUND_STATE, 
	     bool build_ = false);   //!< Normal Constructor      
    isotope (const string & name_, 
	     const string & ch_symbol_, 
	     const int a_,  
	     const int i_ = GROUND_STATE, 
	     bool build_ = false);  //!< Normal Constructor         
    virtual ~isotope ();            //!< Destructor

  private :   /* private attributes */   

    int _z_;    //!<  Number of protons   (0<Z<=119)   
    int _a_;    //!<  Number of nucleons  (Z<=A<=293)  
    int _i_;    //!<  Isomeric states I={0,1,2,3} ={' ','M','N','O'} for "long half-life" time excited states 		    
      
    string _name_;  //!<  Name of the isotope, which should be used as its id ( by default = 'ChA(I)' = ZAI name )    
                  
    double _mass_;     //!<  Mass in gramm per mol [amu]
    double _err_mass_; //!<  Error on the mass in gramm per mol [amu]     
       
    double _half_life_time_;     //!<  Half life time in second [s]
    double _err_half_life_time_; //!<  Error on the half life time in second [s]
      
    bool _is_known_;             //!<  Boolean flag [false by default] = true is (Z,A) values are founded in file mass.mas03
      
    properties _properties_;     //!<  datatools properties         
    
  private :   /* private set & find methods */
      
    void _check_za_ ();       //!<  true is (Z,A) values are founded in file mass.mas03   
      
    void _init_ ();           //!<  Initialize attributes according to new (Z, A, I) values   
      
    void _set_z_ (const int z_);           //!<  Set Z, the number of protons  : 0 <  Z  <= 119  (check if exist)
    void _set_a_ (const int a_);           //!<  Set A,the number of nucleons : Z <= A  <= 293
    void _set_i_ (const int i_);           //!<  Set I, the Isomeric state :  I={0,1,2,3} ={' ','M','N','O'} 

    void _set_name_ (const string & name_);   //!< Set the name             
   
    void _set_mass_ (const double mass_, const double err_mass_=0);          //!<  Set the mass and its error in gramm per mol [g/mol]       
      
    void _set_half_life_time_ (const double half_life_time_ , const double err_half_life_time_);    //!<  Set the half-life time and its error in second [s]     

  public:   

    /* public set & find methods */   
      
    void set_name (const  string & name_);                               //!<  Set the name
    void set_zai (const  int z_, const int a_, const int i_ = 0);        //!<  Set Z, A, I attributes 
      
    void set_mass (const double mass_, const double err_mass_);          //!<  Set the mass in gramm per mol [g/mol] 
    void find_mass (const  string & input_file_name_ = "mass.mas03");    //!<  Search & set the mass from input file (= 'resources/mass.mass03' by default)
       
    void set_decay (const double half_life_time_, const double err_half_life_time_=0);  //!<  Set the decay data 
    void find_decay (const  string & input_file_name_ = "JEFF311RDD_ALL.OUT");          //!<  Search & set the decay data from input file (= 'resources/JEFF311RDD_ALL.OUT' by default)           
  
    /* const retrieval methods */        
       
    const string & get_name () const { return _name_; } //!< Return the name 
    const string & get_ch_symbol () const;              //!< Return the chemical symbol        
    string get_zai_name () const;                       //!< Return the zai name : 'ChA(I)'         
      
    int get_z () const {return _z_;}          //!< Return Z (number of protons)
    int get_a () const {return _a_;}          //!< Return A (number of nucleons)
    int get_i () const {return _i_;}          //!< Return I (isomeric state)     
      
    bool is_known () const; //!< Return _is_known_ boolean flag
      
    bool has_mass_data ()  const ;                    //!< Return true if mass data have been set properly, false either 
    double get_mass ( )    const { return _mass_; }     //!<  Return the mass in gramm per mol [g/mol]
    double get_err_mass () const { return _err_mass_; } //!<  Return the error on the mass in gramm per mol [g/mol]
      
    bool has_decay_data () const; //!< Return true if decay data have been set properly, false either  
    bool is_stable ()     const;  //!< Return true if isotope is stable, false either 
    double get_half_life_time () const {return _half_life_time_;} //!< Return the half-life time in second [s]
    double get_err_half_life_time () const {return _err_half_life_time_;} //!< Return the error on half-life time in second [s]
      
    bool is_locked ()     const; //!< Return true if isotope is locked (i.e. fully set), false either    
    void build () ;              //!< Lock the isotope data structure (i.e. invoking)    

  protected:

    void _build (bool use_decay_);

  public:
    //! Print info      
    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;          

    //! Give access to properties 
    properties & grab_properties ()     { return _properties_; }
                
  }; // end of class isotope

} // end of namespace mat

#endif // __materials__isotope_h

// end of isotope.h
