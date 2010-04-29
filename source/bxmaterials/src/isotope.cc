// -*- mode: c++ ; -*- 
/* isotope.cc
 */

#include <materials/isotope.h>

double endfline_to_double (string endfline_);
double ame3line_to_double (string ame3_line_);

namespace mat {

//________________________________________________________________________// ctor
 isotope::isotope ()
  {         
      __Z = 0; 
      __A = 0;
      __I = 0;
      
      __is_known = false;
      
      __init ();	                
  }
  
//________________________________________________________________________// ctor
 isotope::isotope (const string & name_)
  {               
      /*!
	\param name_ : ID name   

         If name_ is set to '', it will be automatically initialize to 'Ch-A(I)' or '?-0' 
      */ 
      
      __set_name ( name_ );            
      
      __Z = 0; 
      __A = 0;
      __I  = 0;
      
      __is_known = false;
      
      __init ();
      
      	                
  }
  
  
//________________________________________________________________________ // ctor 
  isotope::isotope (const string & name_, const int Z_ , const int A_ , const int I_)
   {  
      /*!
	\param name_ : ID name  
 	\param Z_ : Number of protons  ( 0 <  Z  <= 119)
 	\param A_ : Number of nucleons (Z <= A  <= 293) 
 	\param I_  : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }

	 Default : Isotope in ground state (I=0) 

         If name_ is set to '', it will be automatically initialize to 'Ch-A(I)' or '?-0' 
      */             
      __set_name ( name_ );     
 
       set_ZAI (Z_ , A_, I_);  
   }
   
//________________________________________________________________________ // ctor 
  isotope::isotope(const string & name_, const string & ch_symbol_ , const int A_ , const int I_)
   {             
      /*!
	\param name_ : ID name 
 	\param ch_symbol_ : Chemical symbol ( "H " <=  Ch  <= "Ei")
 	\param A_ : Number of nucleons (Z <= A  <= 293) 
 	\param I_  : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }

	 Default : Isotope in ground state (I=0) 

         If name_ is set to '', it will be automatically initialize to 'Ch-A(I)' or '?-0' 
      */            
         __set_name ( name_ ); 
	  
	  set_ZAI (Z_from_symbol (ch_symbol_) , A_, I_);  
   }

//________________________________________________________________________  dtor: 
 isotope::~isotope ()
  {
  }
  
//________________________________________________________________________  
void isotope::__init ()
   { 
      if( __name.length () == 0  ||  __name.find ("?-0") != string::npos )
      {
       __set_name( get_ZAI_name () );           
      }
      
      __mass=0.; 
      __err_mass=0.; 
     
      __half_life_time = -1.; 
      __err_half_life_time = -1.;
   }   
   
//________________________________________________________________________  
void isotope::__check_ZA ()
   {
      __is_known = false; 
      
    //-----------------  Open an ifstream from file mass.mas03  ----------------------------  

       string tape_name;                                                                                                                               
     
       if (getenv("MATERIALS_ROOT")==NULL)
        { 
           ostringstream message;
	   message << "isotope::__check_ZA : env. variable '$MATERIALS_ROOT'  not found !";
	   throw runtime_error (message.str ());
         }
       else
         { 
	   tape_name.assign(getenv("MATERIALS_ROOT"));
	   tape_name +="/resources/mass.mas03";
	 }

        ifstream ifstr_tape; 
        ifstr_tape.open(tape_name.c_str()); 
        if(!ifstr_tape.is_open ()) 
           {
              ostringstream message;
	      message << "isotope::__check_ZA() : ifstream  '" << tape_name<< "'  not found !";
	      throw runtime_error (message.str ());
	    }

      
      //----------------------------  Read the ifstream  ----------------------------  

	__is_known  = false;  

	string ame03_line;
	
	for(int i = 0 ; i<=39 ; i++) getline(ifstr_tape,ame03_line);

	while( !__is_known && getline(ifstr_tape,ame03_line) )
          {  
	    if( atoi((ame03_line.substr(9,5)).c_str()) == __Z  &&  atoi((ame03_line.substr(14,5)).c_str()) == __A )  
	     {  
		__is_known =true ;
	     }
	  }

	  if(!__is_known) 
            {
               ostringstream message;
	       message<< "isotope::__check_ZA(): " << get_name()<<endl;
	       message << "  -> (Z,A) values : (" << __Z<< ","<< __A<< ") not found in database ['" << tape_name<<"'] !"<<endl;
	       throw runtime_error (message.str ());
	    }
   }   

//________________________________________________________________________     
  void isotope::set_name( const string & name_ )
    { 
      /*!
	\param name_ : ID name
      */	            
      __set_name( name_ );
    } 
   
//________________________________________________________________________     
  void isotope::set_ZAI ( const  int Z_, const int A_, const int I_ )
    {
    /*!
 	\param Z_ : Number of protons  ( 0 <  Z  <= 119)
 	\param A_ : Number of nucleons (Z <= A  <= 293) 
 	\param I_  : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }

	 Default : Isotope in ground state (I=0) 
      */     	            
       __set_Z (Z_); 
       __set_A (A_);
       __set_I (I_); 
       
       __check_ZA (); 
	
       __init ();  
    }

//________________________________________________________________________     
  void isotope::set_mass( const  double mass_, const double err_mass_ )
    {  
    
       /*!
 	\param mass_       : Mass in gramm per mol  [g/mol]
	\param err_mass_ : Mass error in gramm per mol  [g/mol]
      */    	            
       __set_mass (mass_, err_mass_); 
    } 
 
    
//________________________________________________________________________      
     void isotope::find_mass (const string & input_file_name_ )
   {   
      /*!
 	\param input_file_name_ : Name of the input data file  
      */    
       
     //-----------------  Open an ifstream from file mass.mas03  ----------------------------  

       string tape_name;                                                                                                                               
     
       if (getenv("MATERIALS_ROOT")==NULL)
        { 
           ostringstream message;
	   message << "isotope::find_mass : env. variable '$MATERIALS_ROOT'  not found !";
	   throw runtime_error (message.str ());
         }
       else
         { 
	   tape_name.assign(getenv("MATERIALS_ROOT"));
	   tape_name +="/resources/";
	   tape_name += input_file_name_;
	 }

        ifstream ifstr_tape; 
        ifstr_tape.open(tape_name.c_str()); 
        if(!ifstr_tape.is_open ()) 
           {
              ostringstream message;
	      message << "isotope::find_mass() : ifstream  '" << tape_name<< "'  not found !";
	      throw runtime_error (message.str ());
	    }

      
      //----------------------------  Read the ifstream  ----------------------------  

	bool is_za_found  = false;  
        __mass=-1.; 
        __err_mass=-1.;  
   	
	string ame03_line;
	
	for(int i = 0 ; i<=39 ; i++) getline(ifstr_tape,ame03_line);

	while( !is_za_found && getline(ifstr_tape,ame03_line) )
          {  
	    if( atoi((ame03_line.substr(9,5)).c_str()) == __Z  &&  atoi((ame03_line.substr(14,5)).c_str()) == __A )  
	     {  
		is_za_found =true ;
	        
		__set_mass(ame3line_to_double(ame03_line.substr(96,4)) + ame3line_to_double(ame03_line.substr(100,10))*1.E-6 
		                 ,  ame3line_to_double(ame03_line.substr(113,10))*1.E-6);
	     }
	  }

	  if(!is_za_found) 
            {
                // ostringstream message;
	        // message << "isotope::find_mass(): Z A values : '" << __Z<< " "<< __A<< "' not found in mass.mas03 !";
	        // throw runtime_error (message.str ());
	         cerr<<endl<< endl<< "!!! WARNING !!! isotope::find_mass(): Z A values : '" << __Z<< " "<< __A<< "' not found in file mass.mas03 !"<< endl<< endl;
	    }
	
    }    
     
//________________________________________________________________________     
  void isotope::set_decay( const double half_life_time_,  const double err_half_life_time_ )
    { 	
    
     /*!
 	\param half_life_time_       : Half-life time in second [s]
	\param err_half_life_time_ : Half-life time error in second [s]
      */              
       __set_half_life_time(half_life_time_,err_half_life_time_ );
    }    
    
    
    
 //________________________________________________________________________
 const string isotope::get_ch_symbol () const 
   {  
     return mat::symbol_from_Z(__Z); 
    } 
      
 //________________________________________________________________________ 
 const string isotope::get_ZAI_name ()  const 
  {
  
    if( __Z ==0 && __A ==0 && __I ==0)
     {
       return "?-0";  
     }
     
   else
   {
    // Chemical symbol :
    
      string ZAI_name = get_ch_symbol ();
   
    // Number of nucleons : 
      
      ZAI_name+="-";  
      std::stringstream s_A; 
      s_A<<__A; 
      ZAI_name += s_A.str();
   
    // Isomeric state   
  
       if(__I>0)
        {
	   char I_buffer[][4] = { " ", "M", "N", "O"}; 
	   ZAI_name+= "(";
	   ZAI_name+= I_buffer[__I];
	   ZAI_name+= ")";
	}
	return ZAI_name; 
     }  
   }         
 
  
//________________________________________________________________________     
  void isotope::__set_Z( const  int Z_)
    { 	
      /*!
 	\param Z_ : Number of protons  ( 0 <  Z  <= 119)
      */   
      
      if(  Z_ >  0  &&  Z_ <= 119 )
        {
          __Z = Z_;
        }
       else
        {
           ostringstream message;
	   message<< "isotope::__set_Z(): " << get_name()<<endl;
	   message << "       -> invalid value : '" << __Z<< "' !"<<endl;
	   throw logic_error (message.str ());
        }   
   } 
//________________________________________________________________________     
   void isotope::__set_A( const  int A_)
    { 	
       /*!
 	\param A_ : Number of nucleons  ( 0 <  Z  <= 119)
      */   
      
      if(  A_ >= __Z  &&  A_ <= 293)
        {
         __A = A_;
        }
       else
        {
          ostringstream message;
	  message << "isotope::__set_A() : Invalid A value : '" << A_ << "' !";
	  throw logic_error (message.str ());
        }   
    }
//________________________________________________________________________     
   void isotope::__set_I( const  int I_)
    { 	
      /*!
 	\param I_ : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }
      */ 
      
      if(  I_ >= 0  &&  I_ <=3 )
        {
         __I = I_;
        }
       else
        {
          ostringstream message;
	  message << "isotope::__set_I() : Invalid I value : '" << I_ << "' !";
	  throw logic_error (message.str ());
        }   
    } 

//________________________________________________________________________
  void isotope::__set_name(const string & name_)
   {  
     /*!
 	\param name_ : ID name
      */      
     __name = name_;
    }    
 
//________________________________________________________________________ 	       
  void  isotope::__set_mass(const double mass_, const double err_mass_)
   {     
     if(  mass_ < 0.  || err_mass_ <0.)    
       {
          ostringstream message;
	  message << "isotope:::__set_mass() : Invalid mass (+_ error) values : '" << mass_ << "' '" << err_mass_ << " '!";
	  throw logic_error (message.str ());
	}	
     else
      { 
	  __mass = mass_;   
	  __err_mass = err_mass_;   
	} 
    }

//________________________________________________________________________       
   void   isotope::__set_half_life_time(const double half_life_time_ , const double err_half_life_time_)
     { 	
	 if(  half_life_time_ < 0. || err_half_life_time_ <0. )    
          {
            ostringstream message;
	    message << "isotope:::__set_half_life_time() : Invalid half_life_time value : '" 
	                  << half_life_time_  << " +- '" 
	                  << err_half_life_time_<< "' !";
	    throw logic_error (message.str ());
	  }
	 
	 else if(  half_life_time_ == 0.  && err_half_life_time_== 0.  )    
         
	  {
	   __half_life_time = half_life_time_; 
	   __err_half_life_time = err_half_life_time_;   
	 }
	 else
	  {
	   __half_life_time = half_life_time_; 
	   __err_half_life_time = err_half_life_time_;   
	  }
     }
    
//________________________________________________________________________
  void isotope::find_decay(const  string & input_file_name_)
   { 
      /*!
 	\param input_file_name_ : Name of the input data file  
      */      
     //-----------------  Open an ifstream from file JEFF311RDD_ALL.OUT ----------------------------  

       string tape_name;                                                                                                                               
     
       if (getenv("MATERIALS_ROOT")==NULL)
        { 
           ostringstream message;
	   message << "isotope::find_decay : env. variable '$MATERIALS_ROOT'  not found !";
	   throw runtime_error (message.str ());
         }
       else
         { 
	   tape_name.assign(getenv("MATERIALS_ROOT"));
	   tape_name +="/resources/";
	   tape_name +=input_file_name_;
          }
        ifstream ifstr_tape; 
        ifstr_tape.open(tape_name.c_str()); 
        if(!ifstr_tape.is_open ()) 
           {
              ostringstream message;
	      message << "isotope::find_decay() : ifstream  '" << tape_name<< "'  not founded !";
	      throw runtime_error (message.str ());
	    }

      
      //----------------------------  Read the ifstream  ----------------------------  


	bool is_data_found = false; 
	bool is_zai_found  = false;  
        string endf6_I[4]={" ", "M", "N", "O"};  
	
	string endf6_line;
	       
        while( !is_zai_found && getline(ifstr_tape,endf6_line) )
          {                	    
	     if(endf6_line.find("451    5")!=string::npos)
               { 	   
	         if( atoi((endf6_line.substr(0,3)).c_str()) == __Z  &&  atoi((endf6_line.substr(7,3)).c_str()) == __A  
		                                                                      &&  !endf6_I[__I].compare(endf6_line.substr(10,1))    )
	           { 
		      is_zai_found  = true ;
		      /*  symbol  = endf6_line.substr(4,2)    /    data_base = endf6_line.substr(11,7)    /    name = endf6_line.substr(0,11) */
		    } 
	        } 
	     } 
	  if(!is_zai_found) 
            {
              ostringstream message;
	      message<< "isotope::find_decay(): " << get_name()<<endl;
	      message << "       -> Z A I values : '" << __Z<< " "<< __A<< " "<<  __I<< "' not found in '" << tape_name<<"' !"<<endl;
	      throw runtime_error (message.str ());
	    }
	  else
	    { 
	     while( !is_data_found  && getline(ifstr_tape,endf6_line) ) 
		{ 
		   if(endf6_line.find("457    1")!=string::npos)
		     { 
			if((endfline_to_double(endf6_line.substr(0,11)))!=(1000.*get_Z()+get_A()))
		          {
			     ostringstream message;
	                     message << "isotope::find_decay() : ZAI not consistent !";
	                      throw runtime_error (message.str ());
			   } 
			 else
			   {
			      is_data_found  = true ;
			      
			      getline(ifstr_tape,endf6_line);
			      
			       __set_half_life_time( endfline_to_double(endf6_line.substr(0,11)) , endfline_to_double(endf6_line.substr(11,11)));
			     }
	                } 
		   }
		   
		   if(!is_data_found) 
                   {
                  /*  ostringstream message;
	            message << "isotope::find_decay() : 457 data not found in jeff-3.1 for Z A I values : '" << __Z<< " "<< __A<< " "<<  __I<< "' !"<< endl; 
	            throw runtime_error (message.str ()); */
		    cerr << "isotope::find_decay() : 457 data not found in jeff-3.1 for Z A I values : '" << __Z<< " "<< __A<< " "<<  __I<< "' !"<< endl; 
	           } 
	           else
	             {
	            }   
	       }
    }
  
//________________________________________________________________________
const bool isotope::has_mass_data () const {     
     if(__mass > 0. && __err_mass > 0. ) return true;
     else  return false;
  } 

//________________________________________________________________________
const bool isotope::has_decay_data () const {     
     if( __half_life_time >=0. && __err_half_life_time >=0. ) return true;
     else  return false;
  }    


//________________________________________________________________________
const bool isotope::is_stable () const {     
     if( __half_life_time == 0. && __err_half_life_time == 0. ) return true;
     else  return false;
  }    


//________________________________________________________________________
const bool  isotope::is_known () const {     
     return __is_known;
   } 
   
   
//________________________________________________________________________
const bool  isotope::is_locked () const {     
     if(is_known () && has_mass_data () && has_decay_data ()) return true;
     else  return false;
  } 
  
//________________________________________________________________________   
  void isotope::tree_dump (ostream & out_, 
					  const string & title_, 
					  const string & indent_, 
					  bool inherit_) const
  { 
    
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << endl;
      }  
     out_ << indent << i_tree_dumpable::tag 
	 << "name       :'"<< get_name() <<"'" << endl;   
   if(is_known()) 
     {
    out_ << indent << i_tree_dumpable::tag 
	 << "Z,A,I      : " <<get_Z () <<","<<  get_A () <<","<< get_I ()<< ""<<endl; 
    out_ << indent << i_tree_dumpable::tag 
	 << "ZAI name   :'" <<get_ZAI_name() << "'"  <<endl;     
    }
    else 
     {
     out_ << indent << i_tree_dumpable::tag 
	 << "Z,A,I      : "<<  "no data ( default values : "<< get_Z () << ","<< get_A () <<","<< get_I () <<" ) " << endl;     
     out_ << indent << i_tree_dumpable::tag 
	 << "ZAI name   : " <<"no data ( default values : '"<< get_ZAI_name() <<"' ) " << endl;   
     } 
    
    if(has_mass_data()) 
     {
     out_ << indent << i_tree_dumpable::tag 
	 << "mass       : "<<  get_mass () <<" +- " << get_err_mass ()<< " [g/mol]" << endl;  
    } 
    else 
     {
     out_ << indent << i_tree_dumpable::tag 
	 << "mass       : "<<  "no data ( default values : " <<get_mass () <<" +- "<< get_err_mass () <<" [g/mol] ) " << endl;  
     }
    
    if(has_decay_data())
      {
         if( is_stable () )  out_ << indent << i_tree_dumpable::tag << "stable     : true"   << endl;
    
         if(! is_stable() )
          { 
           out_ << indent << i_tree_dumpable::tag << "unstable   :" << endl; 
           out_ << du::i_tree_dumpable::skip_tag << "  `-- T1/2 : "
                  <<  get_half_life_time() <<" +- " << get_err_half_life_time()     <<" [s]"<<endl;
           }
       }
       else
       {
        out_ << indent << i_tree_dumpable::tag 
	 << "stability  : "<<  "no data ( default values : ";
	if(is_stable () ) out_ << "stable & ";
	if(! is_stable () )out_ << "unstable & ";
	 out_<< " T1/2 = "<<get_half_life_time()<<" +- "<<get_half_life_time()<<" [s] )" << endl; 
       }
      
      if(__properties.size () != 0)  
       {
        out_ << indent << du::i_tree_dumpable::tag<< "properties : "<<endl; 
         ostringstream indent_oss;
         indent_oss << indent;
         indent_oss << du::i_tree_dumpable::skip_tag;
         __properties.tree_dump (out_,"",indent_oss.str ());
        }
	else
	{
        out_ << indent << i_tree_dumpable::tag 
	 << "properties : "<<  "empty" << endl; 
       }
           
    
     out_ << indent << i_tree_dumpable::tag 
	 << "locked     : " << (is_locked()? "Yes": "No") << endl;
    
    return;
  }

} // end of namespace mat


//________________________________________________________________________
double endfline_to_double(string endf_line)
{
//  Convert a string (of size 11) written in endf6 format to a double 
//  3 differents formats  
	
	string s_number;	
	if(endf_line.find("E")==string::npos)
	  {	
	        int pos;
		if(endf_line.find("+")!=string::npos) pos=endf_line.find_first_of("+");
		if(endf_line.find("-")!=string::npos)  pos=endf_line.find_first_of("-");
		s_number=endf_line.substr(0,pos)+"E";
		s_number+=endf_line.substr(pos,endf_line.size()-pos);
	 }
	 else s_number = endf_line; 
	 
	 std::istringstream i(s_number);
        
	double number;
        if (!(i >> number))   return 0;
	return number;
}

//________________________________________________________________________
double ame3line_to_double(string ame3_line)
{    
     string s_number;

      if( (ame3_line.find("*")) !=string::npos)  s_number="0";
      else if( (ame3_line.find("#")) !=string::npos)  s_number=ame3_line.substr(0,ame3_line.find_first_of("#"));
      else   s_number = ame3_line;
   
      std::istringstream i(s_number);

        
      double number;

      if (!(i >> number))   return 0; 
      else return number;
}
