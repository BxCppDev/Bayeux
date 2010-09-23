// -*- mode: c++ ; -*- 
/* element.cc
 */

#include <materials/element.h>

namespace mat {

  //________________________________________________________________________// iso_entry ctor
  iso_entry::iso_entry ()
  {
    owned = false;
    iso_ptr = 0;
    weight = -1.0;
  }
      
      
  //________________________________________________________________________// iso_entry dtor      
  iso_entry::~iso_entry ()
  {
    if(iso_ptr != 0){
      if (owned) delete iso_ptr;
      iso_ptr = 0;
    }
    owned = false;  
    weight = -1.0; 
  }
     
     
  //________________________________________________________________________//  iso_entry is_valid ()    
  bool iso_entry::is_valid () const
  {
    return weight > 0.0;
  }   
       

  //________________________________________________________________________//
  void element::_lock_check (const string & where_) const
  {
    if (is_locked ())
      {
	ostringstream message;
	if (! where_.empty ())
	  {
	    message << where_ << ": ";
	  }
	message << "Operation not allowed ! Object is locked !";
	throw runtime_error (message.str ());
      }
    return;
  }
       
  //________________________________________________________________________// element ctor
  element::element ()
  {
    __locked = false;
    __molar_mass = -1.;   
    __name   = "?";
    __symbol = "?";
    __Z = Z_UNDEFINED;
  }


  //________________________________________________________________________// element ctor
  element::element (const string & name_, int Z_, bool build_)
  {
    __locked = false;
    __molar_mass = -1.;   
    __name   = "?";
    __symbol = "?";
    __Z = Z_UNDEFINED;
    set_name (name_);
    set_Z (Z_);         
    if (build_) build ();
  }
  
  //________________________________________________________________________// element ctor
  element::element (const  string & name_, const string & symbol_, bool build_)
  {
    __locked = false;
    __molar_mass = -1.;   
    __name   = "?";
    __symbol = "?";
    __Z = Z_UNDEFINED;
    set_name (name_);
    set_Z (symbol_);        
    if (build_) build ();
  }
  

  //________________________________________________________________________// /dtor 
  element::~element ()
  {
    __composition.clear ();
  } 

  //________________________________________________________________________ 	     
  /*
  void   element::__init()
  { 	
    __unlock ();    
 	
    __composition.clear ();
	
    __molar_mass = -1.;   
  }
  */

  //________________________________________________________________________ 	     
  void   element::__lock ()
  { 	
    __locked = true;  
  }   

  //________________________________________________________________________ 	     
  void   element::__unlock ()
  { 	
    __locked = false; 
  }

  //________________________________________________________________________ 
  /*
  void element::set_symbol (const string & symbol_)
  { 	
    _lock_check ("element::set_symbol");
    __symbol = symbol_; 
    return;
  } 
  */
 
  //________________________________________________________________________ 
  void element::set_name (const string & name_)
  { 	
    _lock_check ("element::set_name");
    __name = name_; 
    return;
  } 
    
  //________________________________________________________________________ 
  void element::set_Z (const string & symbol_)
  {     
    _lock_check ("element::set_Z");
     
    bool is_symbol_found = false; 
    int i_Z = 0;
       
    while (i_Z < chemical_symbol::NB_CHEMICAL_SYMBOLS) 
      {
        if (symbol_ == chemical_symbol::table[i_Z]) 
	  {
	    is_symbol_found = true;
	    break;
	  }
        i_Z++;
      }
      
    if(! is_symbol_found)
      {
	ostringstream message;
	message << endl << "element::set_symbol() ; symbol '" 
		<< symbol_<< "' not found !" << endl;
	throw runtime_error (message.str ());
      }	 	      
    __symbol = symbol_; 
    __Z = i_Z;
    return;
  } 
  //________________________________________________________________________ 
  void element::set_Z (const int Z_)
  {
    _lock_check ("element::set_Z");
     	
    if ((Z_ >= 0) && (Z_ <= chemical_symbol::NB_CHEMICAL_SYMBOLS))
      {
	__Z = Z_;
	__symbol = chemical_symbol::table[__Z];
      }
    else
      {
	ostringstream message;
	message << "element::__set_Z () : Invalid Z value : '" << Z_ << "' !";
	throw logic_error (message.str () );
      }   
    return;
  }
    
  //________________________________________________________________________ 
  void element::add_isotope (const isotope & iso_ptr_,  double weight_,  bool owned_)
  {        
    _lock_check ("element::add_isotope");

    // set Z and symbol when add a new isotope.
       
    if ( __composition.size () == 0) 
      { 
	if (__Z != Z_UNDEFINED)
	  {
	    if (__Z != iso_ptr_.get_Z ())
	      {
		ostringstream message;
		message << "element::add_isotope : Unmatching mother element/daughter isotope Z values !";
		throw logic_error (message.str () );		
	      }
	  }
	set_Z (iso_ptr_.get_Z ());
      } 

    // check if the isotope is locked (which mean valid)     
    
    if (! iso_ptr_.is_locked ())
      {
	ostringstream message;
	message << endl << "element::add_isotope: Isotope '" 
		<< iso_ptr_.get_name ();
	message << "' is not locked !'";
	throw logic_error (message.str ()); 
      }
	      
    // check if Z value of isotope to be added is consistant with Z value of the element        
       
    if ( iso_ptr_.get_Z () != __Z) 
      { 
	ostringstream message;
	message << endl << "element::add_isotope: the Z value '"
		<< iso_ptr_.get_Z () <<"' of isotope '";
	message << iso_ptr_.get_name () 
		<< "' to be added is not consistant with the Z value '" 
		<< __Z << "' of the element '";
	message << get_name () << "' !";
	throw logic_error (message.str ()); 
      }

    // check the positive value of weight
	
    if ( weight_ <= 0.)
      {
	ostringstream message;
	message << endl << "element::add_isotope: weight value '" << weight_ ;
	message << "' is not positive !" << endl ;
	throw logic_error (message.str ());
      }
	      
    // check if isotope is already in the composition of the element  
	   
    if (__composition.find (iso_ptr_.get_name ()) != __composition.end ())
      {
	ostringstream message;
	message << endl << "!! warning !! element::add_isotope() you add the isotope '" << iso_ptr_.get_name ();
	message <<"' which is already in the composition of element '" << __name <<"' !" << endl;
	clog << message.str ();		
      }

	      
    iso_entry entry;
    entry.owned = false;
    entry.iso_ptr = &iso_ptr_;
    entry.weight = weight_;

    __composition[entry.iso_ptr->get_name ()] = entry;
    __composition[entry.iso_ptr->get_name ()].owned = owned_; 	    
    return;
  }
    

  //________________________________________________________________________ 
  void  element::add_isotope (const isotope * iso_ptr_, double weight_)
  {
    add_isotope (*iso_ptr_, weight_, true);
  }      


  //________________________________________________________________________ 
  void element::build ()
  {    
    if (is_locked ()) return;
    if (__composition.size () > 0)
      {
	__norm_weights ();
	__compute_molar_mass ();
      }
    else
      {
	throw runtime_error ("element::build: Missing isotope(s) in the current element ! Not implemented yet !");
	//build_from_nist ();
      }
    __lock ();
  }
    
  //________________________________________________________________________   
  void   element::build_from_nist ()
  {
    
    //-----------------  Open an ifstream from file atomic_weight_nist.dat  ----------------------------  

    string tape_name;                                                                                                                               
     
    if (getenv("MATERIALS_ROOT")==NULL)
      { 
	ostringstream message;
	message << "element::build_from_nist : env. variable '$MATERIALS_ROOT'  not found !";
	throw runtime_error (message.str ());
      }
    else
      { 
	tape_name.assign (getenv ("MATERIALS_ROOT"));
	tape_name +="/resources/isotopic_compositions_nist.dat";
      }

    ifstream ifstr_tape; 
    ifstr_tape.open (tape_name.c_str ()); 
    if(! ifstr_tape.is_open ()) 
      {
	ostringstream message;
	message << "element::build_from_nist : ifstream  '" 
		<< tape_name << "'  not open !";
	throw runtime_error (message.str ());
      }
      
    //----------------------------  Read the ifstream  ----------------------------  

    bool is_z_found  = false;  
    __composition.clear();
    __molar_mass = -1.;
   	
    string nist_line;
	
    for (int i = 0 ; i<=74 ; i++) getline(ifstr_tape,nist_line);
    /*
    bool stop = false;
    while (! stop)
      {
	string tmp;
	getline (ifstr_tape, tmp);
	istringstream iss (tmp);
	string word;
	iss >> word;
	if (word.empty ()) continue;
	else if (word[0] != '#') continue;
	else stop = true;
      }
    */

    while( !is_z_found && getline(ifstr_tape,nist_line) )
      {  
	if( atoi ((nist_line.substr (0, 4)).c_str ()) == __Z && nist_line.find (chemical_symbol::symbol_from_Z (__Z)) != string::npos)  
	  {  
	    cout << nist_line.substr (4, 4) << endl;
	    is_z_found = true ;
	  }
      }
	   
    if(! is_z_found) 
      {
	ostringstream message;
	message << "!!! warning !! element::build_from_nist () : Z values  '" 
		<< __Z 
		<< "' not found in file isotopic_compositions_nist.dat !" 
		<< endl;
	// throw runtime_error (message.str ());
	clog << message << endl;
      }        
  }              
//________________________________________________________________________ 
  void element::__norm_weights ()
  {  
    double norm =0.;     
      
    for (isotope_weight_map_t::const_iterator i = __composition.begin (); 
	 i != __composition.end (); 
	 i++)
      {
	const iso_entry & e = i->second;
        norm += e.weight;
      }
    
    if(norm <= 0.)
      {  
	ostringstream message;
	message << endl << "element::__norm_weights()  sum of weights for isotope '";
	message << get_name () <<"' is negative!'" <<endl ;
	throw logic_error (message.str ());
      }
    
    if(norm != 1.0)
      {  
	/*  ostringstream message;
	    message << endl << "warning !! element::__norm_weights() renormalize weights of element  '";
	    message << get_name () <<"'!'"<< endl;
	    clog << message.str (); 
	*/	
       
	for (isotope_weight_map_t::iterator i = __composition.begin ();
	     i !=  __composition.end (); 
	     i++)
	  {
	    iso_entry & e = i->second;
	    e.weight /= norm;
	  }
      }
  }  
    
    
//________________________________________________________________________ 	       
  void element::__set_molar_mass(const double molar_mass_)
    {      
      if(  molar_mass_ < 0 )    
        {
	  ostringstream message;
	  message << endl << "element:::__set_molar_mass() : Invalid mass value : '" 
		<< molar_mass_ << "' !" << endl;
	  throw logic_error (message.str ());
        }	
        __molar_mass = molar_mass_;   
     }

//________________________________________________________________________ 
  void  element::__compute_molar_mass ()
  {    
    double molar_mass =0.; 
      
    for (isotope_weight_map_t::const_iterator i = __composition.begin (); 
	 i != __composition.end (); 
	 i++)
      { 
	const iso_entry & entry = i->second;
        molar_mass += (entry.weight) * (entry.iso_ptr->get_mass ());
      }
    __set_molar_mass (molar_mass);
  }     

        
           
  //________________________________________________________________________   
  void element::tree_dump (ostream & out_, 
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
	 << "Name         : \""<< get_name() <<"\"" << endl;   
    
    out_ << indent << i_tree_dumpable::tag 
	 << "Chem. symbol : \""<< get_symbol() <<"\"" << endl;  
	 
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Composition  : "; 
    if ( __composition.size () == 0)  
      {
	out_ << "<empty>" << endl;
      }
    else
      {  
        out_ << endl; 
	for (isotope_weight_map_t::const_iterator i = __composition.begin (); 
	     i !=  __composition.end (); 
	     i++)
	  {
	    const string & key = i->first;
	    const iso_entry & entry = i->second;
	    isotope_weight_map_t::const_iterator j = i;
	    j++;
	    string atag = du::i_tree_dumpable::TAG;
	    if (j == __composition.end ()) atag = du::i_tree_dumpable::LAST_TAG;
	    out_  << indent 
		  << i_tree_dumpable::skip_tag 
		  << atag 
		  << "Isotope '" << entry.iso_ptr->get_name () << "'" << " : " 
		  << entry.weight << " " 
		  << (entry.owned? "(owned)": "(not owned)") << endl;
	  }
      } 

    out_ << indent << i_tree_dumpable::tag 
	 << "Molar mass   : " <<  __molar_mass << " [g/mol]" << endl;  

    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Properties   : ";
      if ( __properties.size () == 0) 
	{
	  out_ << "<empty>" <<endl; 
	}
      else
        {
	  out_ << endl; 
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __properties.tree_dump (out_, "", indent_oss.str ());
        }
    }     
	    
     
    out_ << indent << i_tree_dumpable::last_tag 
	 << "Locked       : " << (is_locked()? "Yes": "No") << endl;

    return;
  }
  
  
} // end of namespace mat

// end of element.cc
