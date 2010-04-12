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
    
    __molar_mass = -1.;  
     
     __unlock();
    
     __composition.clear();
  }


//________________________________________________________________________// element ctor
  element::element (const  string name_)
  {
       __name   = name_;
       __symbol = "";
       __Z =0;
    
    __molar_mass = -1.;  
     
     __unlock();
    
     __composition.clear();
  }
  

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
         __unlock();
	 __name = name_;  
	 __molar_mass = -1.;    
    } 
   
//________________________________________________________________________ 
 void element::add_isotope (const isotope & iso_ptr_,  double weight_,  bool owned_)
   {        
         if ( is_locked() )
	  {  
	    ostringstream message;
	    message<< endl << "warning !! element::add_isotope() you add an isotope to a lock element '";
	    message<< get_name ()   <<"'!'"<< endl<< endl;
	    clog << message.str ();

	    __molar_mass = -1.;  	       
	    __unlock();
	  }

       
       if ( __composition.size()== 0.) 
          {     
	    __Z  = iso_ptr_.get_Z();
	   __symbol = iso_ptr_.get_symbol();
	  } 
      
      else 
	{   
	   if ( iso_ptr_.get_Z()!= __Z) 
	    { 
	      ostringstream message;
	      message<< endl << "element::add_isotope() : Z value '" << iso_ptr_.get_Z() <<"' of isotope '";
              message<< iso_ptr_.get_name() << "'is not consistant with Z value '" << __Z << "' of element '";
	      message<< get_name() <<"' !"<< endl << endl;
	      throw logic_error (message.str ()); 
	    }
	    
	   if (__composition.find (iso_ptr_.get_name ()) != __composition.end ())
              {
	       ostringstream message;
	       message<< endl << "element::add_isotope isotope '"<< iso_ptr_.get_name ();
	       message<<"' is already in the composition of element '" << __name <<"' !" << endl<< endl;
	        throw logic_error (message.str ()); 
              }
	      
	    if ( weight_<= 0.)
              {
	       ostringstream message;
	       message<< endl << "element::add_isotope() : weight value '" << weight_ ;
	        message<<"' is not positive !"<< endl << endl;
	       throw logic_error (message.str ());
              }
	      
	     if (!iso_ptr_.is_locked())
              {
	       ostringstream message;
	       message << endl<< "element::add_isotope() isotope '"<< iso_ptr_.get_name ();
	       message<<"' is not locked !'"<< endl<< endl;
	       throw logic_error (message.str ()); 
              }
	   }
      
        iso_entry entry;
        entry.owned = false;
        entry.iso_ptr = &iso_ptr_;
        entry.weight = weight_;

        __composition[entry.iso_ptr->get_name ()] = entry;
        __composition[entry.iso_ptr->get_name ()].owned = owned_; 
    }
    

//________________________________________________________________________ 
void element::add_isotope (const isotope * iso_ptr_, double weight_){
   add_isotope (*iso_ptr_, weight_, true);
}      


//________________________________________________________________________ 
void  element::build ()
    {    
          __norm_weights ();
          __compute_molar_mass ();
	  __lock();
    }  
      
//________________________________________________________________________ 
void element::__norm_weights ()
  {  
    double norm =0.;     
      
    for (isotope_weight_map_t::const_iterator i = __composition.begin (); i !=  __composition.end (); i++)
     {
       const iso_entry & e = i->second;
        norm += e.weight;
       }
    
    if(norm<=0.)
     {  
         ostringstream message;
	 message << endl << "element::__norm_weights()  sum of weights for isotope '";
	 message << get_name () <<"' is negative!'" <<endl << endl;
	 throw logic_error (message.str ());
     }
    
    if(norm!=1)
     {  
        ostringstream message;
	message << endl << "warning !! element::__norm_weights() renormalize weights of element  '";
	message << get_name () <<"'!'"<< endl<< endl;
	clog << message.str (); 
       
       for (isotope_weight_map_t::iterator i = __composition.begin (); i !=  __composition.end (); i++)
         {
           iso_entry & e = i->second;
           e.weight/=norm;
         }
      }
    }  
    
    
//________________________________________________________________________ 	       
  void element::__set_molar_mass(const double molar_mass_)
   {      
     if(  molar_mass_ < 0 )    
       {
          ostringstream message;
	  message << endl <<"element:::__set_molar_mass() : Invalid mass value : '" << molar_mass_ << "' !"<< endl;
	  throw logic_error (message.str ());
	}	
	   __molar_mass = molar_mass_;   
   }

//________________________________________________________________________ 
void  element::__compute_molar_mass ()
    {    
      double molar_mass =0.; 
      
      for (isotope_weight_map_t::const_iterator i = __composition.begin (); i !=  __composition.end (); i++)
       { 
       const iso_entry & entry = i->second;
        molar_mass += (entry.weight)*(entry.iso_ptr->get_mass ());
       }
	 __set_molar_mass(molar_mass);
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
	       out_ << i_tree_dumpable::skip_tag<< du::i_tree_dumpable::tag<< "'"<<entry.iso_ptr->get_name ()<< "'" <<" : "<<
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
