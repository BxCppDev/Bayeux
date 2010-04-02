// -*- mode: c++ ; -*- 
/* element.cc
 */

#include <materials/element.h>

namespace mat {

//________________________________________________________________________// iso_entry ctor
  element::iso_entry::iso_entry ()
   {
      owned = false;
      iso_ptr = 0;
      weight = -1.0;
    }
      
      
//________________________________________________________________________// iso_entry dtor      
   element::iso_entry::~iso_entry ()
     {
       if(iso_ptr != 0){
          if (owned) delete iso_ptr;
          iso_ptr = 0;
         }
	owned = false;  
        weight = -1.0; 
     }
     
     
//________________________________________________________________________//  iso_entry is_valid ()    
  bool element::iso_entry::is_valid () const
    {
      return weight > 0.0;
    }   
       
       
//________________________________________________________________________// element ctor
  element::element ()
  {
     __name   = "";
     __symbol = "";
     __Z =0;
    
    __molar_mass = 0.;  
     
     __unlock();
    
     __composition.clear();
  }

/*
//________________________________________________________________________// element ctor
  element::element (const  string name_,  map_isotope_weight  & map_isotope_weight_)
  {
       __name   = name_;
       
       __symbol = "";
       __Z =0;
      
       __molar_mass = 0.;      
      
       __unlock();
      
       set_composition(map_isotope_weight_);  
      
       __norm_weights ();   
      
       __compute_molar_mass (); 
  }
  
 */
//________________________________________________________________________// /dtor 
  element::~element ()
  {
      __composition.clear();
  }
 

//________________________________________________________________________ 	     
   void   element::__lock()
    { 	
	 __locked = true;  
    }
    

//________________________________________________________________________ 	     
   void   element::__unlock()
    { 	
	 __locked = false; 
    }
 
//________________________________________________________________________ 
   void element::set_name(const string & name_)
    { 	
	 __name = name_;   
    }


//________________________________________________________________________ 	       
   void element::__set_molar_mass(const double molar_mass_)
   {      
     if(  molar_mass_ < 0 )    
       {
          ostringstream message;
	  message << "element:::__set_molar_mass() : Invalid mass value : '" << molar_mass_ << "' !";
	  throw logic_error (message.str ());
	}	
	   __molar_mass = molar_mass_;   
   }
    
//________________________________________________________________________ 
 /*    void element::set_composition (map_isotope_weight & composition_)
   { 
     
        if ( __composition.size()!= 0.) 
        {

	
	}
      
      for (map_isotope_weight::const_iterator it =composition_.begin() ; it != composition_.end() ; it++) 
        { 
	   __add_isotope ((*(it->first)) , (it->second)); 
	}

      __norm_weights ();
     
      __compute_molar_mass (); 
      	 
      __lock();
      
    }
 */      
//________________________________________________________________________ 
   void element::add_isotope (const isotope & iso_ptr_,  double weight_,  bool owned_)
    { 
    
      iso_entry entry;
      entry.owned = false;
      entry.iso_ptr = &iso_ptr_;
      entry.weight = weight_;

     __composition[entry.iso_ptr->get_name ()] = entry;
     __composition[entry.iso_ptr->get_name ()].owned = owned_;
/*
//  Set or check the Z of the element :

       if ( __composition.size()== 0.) 
        {
            
	  __Z  = isotope_.get_Z();
	  __symbol = isotope_.get_symbol();
	
	} 
	else 
	{   
	   if ( isotope_.get_Z()!= __Z) 
	    { 
	      ostringstream message;
	      message << "element::add_isotope() : Z value '" << isotope_.get_Z() <<"' of isotope '" 
	      << isotope_.get_name() << "'is not consistant with Z value '" << __Z << "' of element '"<< get_name() <<"' !" << endl;
	      throw logic_error (message.str ()); 
	    }
	}

//  check the weight (must be positive) :

      if ( weight_<= 0.)
         {
	   ostringstream message;
	   message << "element::add_isotope() : weight value '" << weight_ <<"' not positive !" << endl;
	   throw logic_error (message.str ());
         }
           
//  check if isotope is lock (which mean exist) :
       
       if ( !isotope_.is_locked())
          {
	   ostringstream message;
	   message << "element::add_isotope() : isotope '" << isotope_.get_name() <<"' is not lock  !" << endl;
	   throw logic_error (message.str ());
          }
 
     
//  check if isotope already in use in the composition :  
     
         bool is_isotope_already_use = false;	
	  
	 if(__composition.count(&isotope_)>0)
           {
             is_isotope_already_use = true;
	   cerr<<endl<< "!!! WARNING !!! element::add_isotope() : isotope '"<< isotope_.get_name()<<"' is already in the  composition of  '"<< get_name()<<"' !"<< endl<<endl;     
	   }

	  if(!is_isotope_already_use)
	    {
	     for (map_isotope_weight::const_iterator it =__composition.begin() ; it != __composition.end() ; it++) 
               { 
	        if(  *(it->first)  == isotope_ )
		  {
		    is_isotope_already_use = true;
		    cerr<<endl<< "!!! WARNING !!! element::add_isotope() : isotope '"<< isotope_.get_name()
		    <<"' is already entered in the composition of  '"<< get_name()<<"' !"<< endl<<endl; 
		  }
               }
	    }  
	    
	__composition.insert (std::make_pair(&isotope_,weight_));
    
    */
    }

//________________________________________________________________________ 
void element::add_isotope (const isotope * iso_ptr_, double weight_){
   add_isotope (*iso_ptr_, weight_, true);
}      

//________________________________________________________________________ 
void element::__norm_weights ()
    {  
      /*
      double norm = 0.;
      
      for (map_isotope_weight::const_iterator it =__composition.begin() ; it != __composition.end() ; it++) 
           { 
	     norm+= (it->second); 
           }
       
       if(norm<=0){
          
	  ostringstream message;
	  message << "element::__norm_weights() : norm value '" << norm <<"' not positive !" << endl;
	  throw logic_error (message.str ());
	
	} else {
        
	    for(map_isotope_weight::iterator it =__composition.begin() ; it != __composition.end() ; it++) 
               { 
	          (*it).second /=norm;
               }
	 }*/
    }  
    
//________________________________________________________________________ 
void  element::__compute_molar_mass ()
    {    
    /*   double molar_mass =0.; 
       
       for (map_isotope_weight::const_iterator it =__composition.begin() ; it != __composition.end() ; it++) 
         { 
	     molar_mass += (it->second)*(*it).first->get_mass(); 
         }
	 
	 __set_molar_mass(molar_mass);
*/
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
	 << "Name       : \""<< get_name() <<"\"" << endl;   
    
    out_ << indent << i_tree_dumpable::tag 
	 << "Ch symbol  : \""<< get_symbol() <<"\"" << endl;  
	 
    out_ << indent << du::i_tree_dumpable::tag << "Composition: "; 
      if ( __composition.size () == 0)  out_ << "<empty>"<< std::endl;
      else
      {  
        out_ << std::endl; 
	
	for (isotope_weight_map_t::const_iterator i = __composition.begin (); i !=  __composition.end (); i++)
           {
              const string & key = i->first;
              const iso_entry & entry = i->second;
	       out_ << i_tree_dumpable::skip_tag<< du::i_tree_dumpable::tag<< "'"<<entry.iso_ptr->get_name ()<< "'" <<"     :  "<<
	          entry.weight<< " " << (entry.owned? "(owned)": "(not owned)") <<std::endl;
                }
        } 

     out_ << indent << i_tree_dumpable::tag 
	 << "Molar mass : "<<  __molar_mass << " [g/mol]" << endl;  
	 
	 
	   {
    out_ << indent << du::i_tree_dumpable::tag
	   << "properties : ";
         if ( __properties.size () == 0)           out_ << "<empty>"<<endl; 

        else
        {
         ostringstream indent_oss;
         indent_oss << indent;
         indent_oss << du::i_tree_dumpable::skip_tag;
         __properties.tree_dump (out_,"",indent_oss.str ());
        }
      }     
	    
     
     out_ << indent << i_tree_dumpable::tag 
	 << "Locked     : " << (is_locked()? "Yes": "No") << endl;

    return;
  }
  
  
} // end of namespace mat

// end of element.cc
