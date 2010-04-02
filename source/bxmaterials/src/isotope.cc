// -*- mode: c++ ; -*- 
/* isotope.cc
 */

#include <materials/isotope.h>

double endfline_to_double(string endfline_);
double ame3line_to_double(string ame3_line_);

string isotope_symbol[119]={"n ", "H ", "He", "Li", "Be", "B ","C ", "N ", "O ", "F ", "Ne",			
"Na", "Mg", "Al", "Si", "P ", "S ", "Cl", "Ar", "K ", "Ca", "Sc", "Ti", "V ", "Cr", "Mn", "Fe", 
"Co", "Ni", "Cu", "Zn", "Ga", "Ge","As", "Se", "Br", "Kr", "Rb", "Sr", "Y ", "Zr", "Nb", "Mo", 
"Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I ", "Xe","Cs", "Ba", "La", "Ce", 
"Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb","Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", 
"W ", "Re", "Os","Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra","Ac", 
"Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", 
"Db", "Sg", "Bh", "Hs", "Mt", "Ds",  "Rg", "Ec", "Ed", "Ee", "Ef", "Eg", "Eh", "Ei" };

namespace mat {

//________________________________________________________________________// ctor
 isotope::isotope ()
  {   
   //--  Identifers --   
     
      __set_Z(0); 
      __set_A(0);
      __set_I(0);
    
      __symbol="??";
      __build_name();  
     
    //--    Initialize others attributes to their default values --       
    
      __set_mass(0.,0.); 
     
      __set_stable(true);
      __rtyp = -1.;
      __set_half_life_time(0.,0.); 
     
      __unlock(); 	                
      __has_decay_data = false; 	 
  }
  
  
//________________________________________________________________________ // ctor 
  isotope::isotope(const int Z_ , const int A_ , const int I_)
   {     
    //--  Identifers --         
     
       __set_Z(Z_); 
       __set_A(A_);
       __set_I(I_);
    
       __find_symbol();
       __build_name(); 
       
     //--    Initialize others attributes to their default values --  
       
       __unlock(); 
       __find_mass();
     
       __set_stable(true);
       __rtyp = -1.;
       __set_half_life_time(0.,0.); 
     	                
       __has_decay_data = false; 	  
   }
   
//________________________________________________________________________ // ctor 
  isotope::isotope(const string & symbol_ , const int A_ , const int I_)
   {     
    //--  Identifers --    
         __symbol = symbol_;
         __find_Z(); 
	      
	 __set_A(A_);
         __set_I(I_);
	 
         __build_name();   
     
    //--    Initialize others attributes to their default values -- 
         
	 __unlock(); 
        __find_mass();
        
	__set_stable(true);
        __rtyp = -1.;
        __set_half_life_time(0.,0.); 
                     
        __has_decay_data = false;    
   }
   
//________________________________________________________________________  dtor: 
 isotope::~isotope ()
  {
  }

//________________________________________________________________________     
  void isotope::__set_Z( const  int Z_)
    { 	
  
      if(  Z_ >=  0  &&  Z_ <= 119 )
        {
          __Z = Z_;
        }
       else
        {
          ostringstream message;
	   message << "isotope::__set_Z() : Invalid Z value : '" << Z_ << "' !";
	   throw logic_error (message.str ());
        }   
   } 
//________________________________________________________________________     
   void isotope::__set_A( const  int A_)
    { 	
  
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
  void isotope::set_ZAI( const  int Z_, const int A_, const int I_ )
    { 	
  
    //  Identifers :         
     
       __set_Z(Z_); 
       __set_A(A_);
       __set_I(I_);
    
       __find_symbol();
       __build_name(); 
     
     
   //  Initialize others attributes to their default values 
    
       __find_mass();
     
      __set_stable(true);
      __rtyp = -1.;
      __set_half_life_time(0.,0.);    
                 
      __has_decay_data = false; 
  } 
  
  
//________________________________________________________________________
  void isotope::__build_name()
   {     
  
  // Build name of the isotope in endf6 format 'ZZZ-Ch-AAAI';
    	  
	  char I_buffer[][4] = { " ", "M", "N", "O"};
	  
	  char buffer [11];
	  sprintf (buffer, "%3d%s%s%s%3d%1s", __Z,"-", __symbol.c_str(),"-",__A,I_buffer[__I]); 
          
	  __name = buffer;
    }   
    
//________________________________________________________________________
  void isotope::__find_symbol()
   {     
      
    if(  __Z >= 1  &&  __Z < 111)
        {
	  __symbol = isotope_symbol[__Z];
	}
     else
        {
	    ostringstream message;
	    message << "isotope::__find_symbol(): Z value : '" << __Z<<"' not tabulated !";
	    throw runtime_error (message.str ());
	}
    }  


//________________________________________________________________________
  void isotope::__find_Z()
   {     
     bool is_symbol_found = false; 
					    
     int i_Z=0;
    
     while(!is_symbol_found && i_Z < 111) 
     {
       if(__symbol == isotope_symbol[i_Z]) is_symbol_found = true;
       i_Z++;
     }
      
      if(!is_symbol_found)
         {
	    ostringstream message;
	    message << "isotope::__find_Z(): symbol value : '" << __symbol<<"' not tabulated !";
	    throw runtime_error (message.str ());
	 }	 
      else __Z = i_Z-1;
    
    }   
//________________________________________________________________________
const bool isotope::operator==(isotope & isotope_)
{
	if(__Z==isotope_.get_Z () && __A==isotope_.get_A () && __I==isotope_.get_I () ) return true;
	return false;
}
//________________________________________________________________________ 	       
  void  isotope::__set_mass(const double mass_, const double err_mass_)
   {      
     if(  mass_ < 0.  || mass_ <0.)    
       {
          ostringstream message;
	  message << "isotope:::set_mass() : Invalid mass or error values : '" << mass_ << "' '" << err_mass_ << "'!";
	  throw logic_error (message.str ());
	}	
     else
      { 
	   __mass = mass_;   
	   __err_mass = err_mass_;   
	} 
    }
    
    
//________________________________________________________________________ 	     
   void   isotope::__set_stable(const bool is_stable_)
    { 	
	 __is_stable = is_stable_;  
	 if(is_stable_)
	 {
	   __half_life_time = 0.; 
	   __err_half_life_time = 0.;   
	 } 
    }

//________________________________________________________________________ 	     
   void   isotope::__lock()
    { 	
	 __locked = true;  
    }
    
//________________________________________________________________________ 	     
   void   isotope::__unlock()
    { 	
	 __locked = false; 
    }

//________________________________________________________________________       
   void   isotope::__set_half_life_time(const double half_life_time_ , const double err_half_life_time_)
     { 	
	 if(  half_life_time_ < 0. || err_half_life_time_ <0. )    
          {
            ostringstream message;
	    message << "isotope:::set_half_life_time() : Invalid half_life_time value : '" 
	                  << half_life_time_  << " +- '" 
	                  << err_half_life_time_<< "' !";
	  throw logic_error (message.str ());
	 }
	 else if(  half_life_time_ == 0.  )    
          {
           __set_stable(true);
	   __half_life_time = half_life_time_; 
	   __err_half_life_time = err_half_life_time_;   
	 }
	 else
	  {
	   __set_stable(false);
	   __half_life_time = half_life_time_; 
	   __err_half_life_time = err_half_life_time_;   
	  }
     }
     
//________________________________________________________________________
  void isotope::__find_mass()
   {     
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
	   tape_name +="/resources/mass.mas03";
	 }

        ifstream ifstr_tape; 
        ifstr_tape.open(tape_name.c_str()); 
        if(!ifstr_tape.is_open ()) 
           {
              ostringstream message;
	      message << "isotope::find_mass() : ifstream  '" << tape_name<< "'  not open !";
	      throw runtime_error (message.str ());
	    }

      
      //----------------------------  Read the ifstream  ----------------------------  

	bool is_za_found  = false;  
	__set_mass(0.,0.);
   	
	string ame03_line;
	
	for(int i = 0 ; i<=39 ; i++) getline(ifstr_tape,ame03_line);

	while( !is_za_found && getline(ifstr_tape,ame03_line) )
          {  
	    if( atoi((ame03_line.substr(9,5)).c_str()) == __Z  &&  atoi((ame03_line.substr(14,5)).c_str()) == __A )  
	     {  
	        is_za_found =true ;
	       __mass = ame3line_to_double(ame03_line.substr(96,4)) + ame3line_to_double(ame03_line.substr(100,10))*1.E-6;
	       __err_mass = ame3line_to_double(ame03_line.substr(113,10))*1.E-6;
	       __lock();
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
  bool isotope::find_decay()
   {     
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
	   tape_name +="/resources/JEFF311RDD_ALL.OUT";
	 }

        ifstream ifstr_tape; 
        ifstr_tape.open(tape_name.c_str()); 
        if(!ifstr_tape.is_open ()) 
           {
              ostringstream message;
	      message << "isotope::find_decay() : ifstream  '" << tape_name<< "'  not open !";
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
		      //  __symbol          = endf6_line.substr(4,2) ;
		      // data_base     = endf6_line.substr(11,7);
		     // __name            = endf6_line.substr(0,11) ;
		    } 
	        } 
	     } 
	  if(!is_zai_found) 
            {
            /* ostringstream message;
	      message << "isotope::find_decay(): Z A I values : '" << __Z<< " "<< __A<< " "<<  __I<< "' not found in jeff-3.1!";
	      throw runtime_error (message.str ());*/
	     cerr<<endl<< endl<< "!!! WARNING !!! isotope::find_decay(): Z A I values : '" << __Z<< " "<< __A<< " "<<  __I<< "' not found in jeff-3.1!"<< endl<<endl;
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
			      __half_life_time = endfline_to_double(endf6_line.substr(0,11));
			       __err_half_life_time = endfline_to_double(endf6_line.substr(11,11)) ;
			       if(__half_life_time!=0.) __is_stable=false ;
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
                     __has_decay_data = true;
	            }   
	       }
	   return(is_data_found);
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
	 << "Name       :'"<< get_name() <<"'" << endl;   

    out_ << indent << i_tree_dumpable::tag 
	 << "Z A I      : " <<  get_Z() <<" "<<  get_A() <<" "<< get_I()<<endl;     
   
     out_ << indent << i_tree_dumpable::tag 
	 << "Mass       : "<<  get_mass() <<" +- " << get_err_mass()<< " [u.m.a.]" << endl;  
    
      if(__has_decay_data)
      {
         if(__is_stable)  out_ << indent << i_tree_dumpable::tag << "Stable     : true"   << endl;
    
         if(!__is_stable)
          { 
           out_ << indent << i_tree_dumpable::tag << "Unstable   :" << endl; 
           out_ << du::i_tree_dumpable::skip_tag << "  `-- T1/2 : "
                  <<  get_half_life_time() <<" +- " << get_err_half_life_time()     <<" [s]"<<endl;
           }
       }
      
      if(__properties.size () != 0)  
       {
        out_ << indent << du::i_tree_dumpable::tag<< "properties : "<<endl; 
         ostringstream indent_oss;
         indent_oss << indent;
         indent_oss << du::i_tree_dumpable::skip_tag;
         __properties.tree_dump (out_,"",indent_oss.str ());
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
