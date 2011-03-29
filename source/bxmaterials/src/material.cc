// -*- mode: c++ ; -*- 
/* material.cc
 */

#include <materials/material.h>
#include <datatools/utils/clhep_units.h>

namespace mat {

  using namespace std;

  const double material::g_per_cm3 = CLHEP::g / CLHEP::cm3;
    
  //________________________________________________________________________
  compound_entry::compound_entry ()
  {
    owned = false;
    
    elt_ptr = 0;
    mat_ptr = 0;
     
    weight          = -1; 
    nb_of_atoms = -1;
  }      
      
  //________________________________________________________________________    
  compound_entry::~compound_entry ()
  {
    if(elt_ptr != 0){
      if (owned) delete elt_ptr;
      elt_ptr = 0;
    }
    
    if(mat_ptr != 0){
      if (owned) delete mat_ptr;
      mat_ptr = 0;
    }    
    
    owned = false;  
    
    weight = -1;  
    nb_of_atoms = -1;
  }
          
  //________________________________________________________________________
  bool compound_entry::is_valid () const
  {
    return ((weight > 0.0) || (nb_of_atoms > 0.0));
  }   

  //________________________________________________________________________ 
  bool material::is_composed_by_mean_z_a () const
  {
    return __proportion_unit == MEAN_ZA;
  }

  //________________________________________________________________________ 
  bool material::is_composed_by_number_of_atoms () const
  {
    return __proportion_unit == NUMBER_OF_ATOMS;
  }

  //________________________________________________________________________ 
  bool material::is_composed_by_fraction_mass () const
  {
    return __proportion_unit == FRACTION_MASS;
  }

  //________________________________________________________________________ 
  const composition_map_t & material::get_composition () const
  {
    return __composition;
  }

  //________________________________________________________________________   ctor:
  material::material ()
  {
    __name = "";
    __proportion_unit = KP_UNDEFINED;
    __locked = false;
    __density = -1.0 * g_per_cm3;
    __mean_z = -1.0;
    __mean_a = -1.0;
  }
  
  //________________________________________________________________________   ctor:
  material::material (const string & name_)
  {
    set_name (name_);
    __density = -1.0 * g_per_cm3;  
    __locked = false;
  }
  
  //________________________________________________________________________   dtor:
  material::~material ()
  {
  }

  //________________________________________________________________________  
  const string & material::get_name () const
  {
    return __name;
  }
  
  //________________________________________________________________________    
  void material::set_name (const string & name_)
  {
    __name = name_;
  }

  //________________________________________________________________________    
  double  material::get_mean_z () const
  {
    return __mean_z;
  }
  
  //________________________________________________________________________    
  void  material::__set_mean_z (double z_)
  {
    if (z_ <= 0.0)
      {
	throw runtime_error ("material::__set_mean_z: Invalid value for atomic number !");
      }
    __mean_z = z_;
    return;
  }
  
  //________________________________________________________________________    
  double  material::get_mean_a () const
  {
    return __mean_a;
  }
  
  //________________________________________________________________________    
  void  material::__set_mean_a (double a_)
  {
    if (a_ <= 0.0)
      {
	throw runtime_error ("material::__set_mean_a: Invalid value for mass number !");
      }
    __mean_a = a_;
    return;
  } 

  //________________________________________________________________________    
  void material::set_mean_z_a (double z_, double a_)
  {
    _lock_check ("material::set_mean_z_a");
    if (__proportion_unit == KP_UNDEFINED)
      {
	__proportion_unit = KP_ZA;
      }
    else
      {
	if (__proportion_unit != KP_ZA)
	  {
	    throw runtime_error ("material::set_mean_z_a: Setting Z/A is not allowed in this mode !");
	  }
      }
    __set_mean_z (z_);
    __set_mean_a (a_);
    return;
  } 
  
  //________________________________________________________________________    
  double material::get_density () const
  {
    return __density;
  }
  
  //________________________________________________________________________  
  void material::set_density (double density_)
  {
    if (density_ <= 0.0)
      {
	throw runtime_error ("material::set_density: Invalid value for density !");
      }
    __density = density_;
  }
  
  //________________________________________________________________________ 
  void material::add_element_by_mass (const element & elt_ref_ , double weight_ , bool owned_ )
  {        
    _lock_check ("material::add_element_by_mass");
   
    // check proportion_unit_type when add a new element 
    // if you are adding the first element then set the proportion_unit 
       
    if ( __composition.size () == 0) 
      { 
	__proportion_unit = KP_MASS;
      } 
    else
      { 
	if(__proportion_unit!=KP_MASS)
	  {
	    ostringstream message;
	    message << "material::add_element_by_mass : Unmatching proportion unit  !";
	    throw logic_error (message.str () );		
	  }
      } 
      
    // check if the element is locked (which mean valid)     
    
    if (! elt_ref_.is_locked ())
      {
	ostringstream message;
	message << endl << "material::add_element_by_mass : Element '" << elt_ref_.get_name ();
	message << "' is not locked !'";
	throw logic_error (message.str ()); 
      }	      

    // check the positive value of weight
	
    if ( weight_ <= 0.)
      {
	ostringstream message;
	message << endl << "material::add_element_by_mass : weight value '" << weight_ ;
	message << "' is not positive !" << endl ;
	throw logic_error (message.str ());
      }	      
	      
    compound_entry entry;
    entry.owned = false;
    entry.elt_ptr = &elt_ref_;
    entry.weight = weight_;

    __composition[entry.elt_ptr->get_name ()] = entry;
    __composition[entry.elt_ptr->get_name ()].owned = owned_; 	    
    return;
  }
    
  //________________________________________________________________________ 
  void  material::add_element_by_mass (const element * elt_ptr_ , double weight_  )
  {
    add_element_by_mass (*elt_ptr_, weight_);
  }
  
  //________________________________________________________________________ 
  void material::add_element_by_nb_of_atoms(const element & elt_ref_ , int nb_of_atoms_ , bool owned_ )
  {        
    _lock_check ("material::add_element_by_nb_of_atoms");
   
    // check proportion_unit_type when add a new element 
    // if you are adding the first element then set the proportion_unit 
       
    if (__composition.size () == 0) 
      { 
	__proportion_unit = KP_ATOM;
      } 
    else
      { 
	if (__proportion_unit != KP_ATOM)
	  {
	    ostringstream message;
	    message << "material::add_element_by_nb_of_atoms : Unmatching proportion unit  !";
	    throw logic_error (message.str () );		
	  }
      } 
      
    // check if the element is locked (which mean valid)     
    
    if (! elt_ref_.is_locked ())
      {
	ostringstream message;
	message << endl << "material::add_element_by_nb_of_atoms : Element '" << elt_ref_.get_name ();
	message << "' is not locked !'";
	throw logic_error (message.str ()); 
      }	      

    // check the positive value of weight
	
    if ( nb_of_atoms_ <= 0.)
      {
	ostringstream message;
	message << endl << "material::add_element_by_nb_of_atoms : nb of atoms value '" << nb_of_atoms_ ;
	message << "' is not positive !" << endl ;
	throw logic_error (message.str ());
      }	      
	      
    compound_entry entry;
    entry.owned = false;
    entry.elt_ptr = &elt_ref_;
    entry.nb_of_atoms = nb_of_atoms_;

    __composition[entry.elt_ptr->get_name ()] = entry;
    __composition[entry.elt_ptr->get_name ()].owned = owned_; 	    
    return;
  }
    
  //________________________________________________________________________ 
  void  material::add_element_by_nb_of_atoms (const element * elt_ptr_ ,  int nb_of_atoms_  )
  {
    add_element_by_nb_of_atoms (*elt_ptr_, nb_of_atoms_);
  }
  
  
  //________________________________________________________________________ 
  void material::add_material_by_mass(const material & mat_ref_ , double weight_ , bool owned_ )
  {        
    _lock_check ("material::add_material_by_mass");
   
    // check proportion_unit_type when add a new element 
    // if you are adding the first element then set the proportion_unit 
       
    if ( __composition.size () == 0) 
      { 
	__proportion_unit = KP_MASS;
      } 
    else
      { 
	if(__proportion_unit != KP_MASS)
	  {
	    ostringstream message;
	    message << "material::add_material_by_mass : Unmatching proportion unit  !";
	    throw logic_error (message.str () );		
	  }
      } 
      
    // check if the element is locked (which mean valid)     
    
    if (! mat_ref_.is_locked ())
      {
	ostringstream message;
	message << endl << "material::add_material_by_mass : Material '" << mat_ref_.get_name ();
	message << "' is not locked !'";
	throw logic_error (message.str ()); 
      }	      

    // check the positive value of weight
	
    if ( weight_ <= 0.)
      {
	ostringstream message;
	message << endl << "material::add_material_by_mass : weight value '" << weight_ ;
	message << "' is not positive !" << endl ;
	throw logic_error (message.str ());
      }	      
	      
    compound_entry entry;
    entry.owned = false;
    entry.mat_ptr = &mat_ref_;
    entry.weight = weight_;

    __composition[entry.mat_ptr->get_name ()] = entry;
    __composition[entry.mat_ptr->get_name ()].owned = owned_; 	    
    
    return;
  }
    
  //________________________________________________________________________ 
  void  material::add_material_by_mass (const material * mat_ptr_ , double weight_  )
  {
    add_material_by_mass (*mat_ptr_, weight_);
  }
  
  //________________________________________________________________________ 
  void material::build ()
  {    
    _lock_check ("material::build");
    if (__density <= 0.0)
      {
	throw runtime_error ("material::build: Invalid density !");
      }
    if (is_composed_by_mean_z_a ())
      {
	if (__mean_z < 0.0)
	  {
	    throw runtime_error ("material::build: Invalid mean atomic number (Z) !");
	  }
	if (__mean_a < 0.0)
	  {
	    throw runtime_error ("material::build: Invalid mean mass number (A) !");
	  }
      }
    else
      {
	if (__composition.size () > 0)
	  {
	    if (is_composed_by_fraction_mass ())
	      {
	    	__normalize_weights ();
	      }
	    __compute_molar_mass ();
	  }
	else
	  {
	    throw runtime_error ("material::build: Missing compound(s) in the current material ! Not implemented yet !");
	  }
      }
    __lock ();
  }

  //________________________________________________________________________ 
  void material::__normalize_weights ()
  {
    if (! is_composed_by_fraction_mass ())
      {
	throw runtime_error ("material::__normalize_weights: Invalid composition mode !");
      }
    if (__composition.size () == 0)
      {
	throw runtime_error ("material::__normalize_weights: Empty composition map !");
      }
    // compute the sum of weight (fraction of mass):
    double sum_weight = 0.0;
    for (composition_map_t::const_iterator i 
	   = __composition.begin (); 
	 i != __composition.end (); 
	 i++)
      { 
	const compound_entry & entry = i->second;
        sum_weight += entry.weight;
      }
    // normalize all weights (fraction of mass):
    for (composition_map_t::iterator i = __composition.begin (); 
	 i != __composition.end (); 
	 i++)
      { 
	compound_entry & entry = i->second;
	entry.weight /= sum_weight;
      }
    return;
  }   
  
  //________________________________________________________________________ 
  void  material::__compute_molar_mass ()
  {    
    /*  double molar_mass =0.; 
      
	for (isotope_weight_map_t::const_iterator i = __composition.begin (); 
	i != __composition.end (); 
	i++)
	{ 
	const iso_entry & entry = i->second;
        molar_mass += (entry.weight) * (entry.iso_ptr->get_mass ());
	}
	__set_molar_mass (molar_mass);
    */
  }     
  
  //________________________________________________________________________ 	     
  void   material::__lock ()
  { 	
    __locked = true;  
  }    
  
  
  //________________________________________________________________________
  void material::_lock_check (const string & where_) const
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
  
  //________________________________________________________________________      
  void material::tree_dump (ostream & out_,  
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
	 << "Name         : \"" << get_name() <<"\"" << endl;   
    
    out_ << indent << i_tree_dumpable::tag 
	 << "Density      : " << get_density() / g_per_cm3 << " g/cm3" << endl;   
	 
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Composition  : "; 
    if (is_composed_by_mean_z_a ())
      {
	out_ << endl; 
	out_ << indent << du::i_tree_dumpable::skip_tag
	     << du::i_tree_dumpable::tag << "Mean Z : " <<__mean_z << endl;
	out_ << indent << du::i_tree_dumpable::skip_tag
	     << du::i_tree_dumpable::last_tag << "Mean A : " << __mean_a << endl;
	
      }
    else
      {
	if ( __composition.size () == 0)  
	  {
	    out_ << " <empty>" << endl;
	  }
	else
	  {  
	    if (__proportion_unit == KP_ATOM)
	      {  
		out_ << "(# atoms) "<< endl;	
		for (composition_map_t::const_iterator i 
		       = __composition.begin (); 
		     i != __composition.end (); 
		     i++)
		  {
		    const string & key = i->first;
		    const compound_entry & entry = i->second;
		    composition_map_t::const_iterator j = i;
		    j++;
		    string atag = du::i_tree_dumpable::TAG;
		    if (j == __composition.end ()) atag = du::i_tree_dumpable::LAST_TAG;   
		    out_ << indent << i_tree_dumpable::skip_tag << atag << "Element '" 
			 << entry.elt_ptr->get_name () << "'" << " : " 
			 << entry.nb_of_atoms << " " 
			 << (entry.owned? "(owned)": "(not owned)") << endl;
		  }
	      }  
	    
	    if (__proportion_unit == KP_MASS) 
	      {  
		out_ << "(% mass)  "<< endl;	
		for (composition_map_t::const_iterator i 
		       = __composition.begin (); 
		     i != __composition.end (); 
		     i++)
		  {
		    const string & key = i->first;
		    const compound_entry & entry = i->second;
		    composition_map_t::const_iterator j = i;
		    j++;
		    string atag = du::i_tree_dumpable::TAG;
		    if (j == __composition.end ()) atag = du::i_tree_dumpable::LAST_TAG;   
		    out_ << indent << i_tree_dumpable::skip_tag << atag;
		    if (entry.mat_ptr == 0) 
		      {
			out_  << "Element '" << entry.elt_ptr->get_name ();
		      }
		    else 
		      {
			out_  << "Material '" << entry.mat_ptr->get_name ();
		      }
		    out_ << "'" << " : " << entry.weight << " " 
			 << (entry.owned? "(owned)": "(not owned)") << endl;
		  }
	      }  		
	  }
      }
    
    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Properties   : ";
      if ( __properties.size () == 0)
	{
	  out_ << "<empty>" << endl; 
	}
      else
        {
	  out_ << endl;
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __properties.tree_dump (out_,"",indent_oss.str ());
        }
    }     
	    
    {
      out_ << indent << i_tree_dumpable::last_tag 
	   << "Locked       : " << (is_locked()? "Yes": "No") << endl;
    }

    return;
    
  }

} // end of namespace mat

// end of material.cc
