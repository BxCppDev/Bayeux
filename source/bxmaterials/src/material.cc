// -*- mode: c++ ; -*- 
/* material.cc
 */

#include <materials/material.h>

namespace mat {

  const double material::g_per_cm3 = CLHEP::g / CLHEP::cm3;

  composite_entry::composite_entry ()
  {
  }

  composite_entry::~composite_entry ()
  {
  }
 
  const string & material::get_name () const
  {
    return __name;
  }
  
  void material::set_name (const string & new_value_)
  {
    __name = new_value_;
  }
  
  
  double material::get_density () const
  {
    return __density;
  }

  void material::set_density (double density_)
  {
    if (density_ <= 0.0)
      {
	throw runtime_error ("material::set_density: Invalid value for density !");
      }
    __density = density_;
  }
  
  // ctor:
  material::material ()
  {
    __name = "";
    __density = 1.0 * g_per_cm3;
  }
  
  // dtor:
  material::~material ()
  {
  }
  
   
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
	 
    /*
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Composition  : "; 
    if ( __composition.size () == 0)  out_ << "<empty>"<< std::endl;
    else
      {  
        out_ << std::endl; 
	
	for (isotope_weight_map_t::const_iterator i = __composition.begin (); i !=  __composition.end (); i++)
	  {
	    const string & key = i->first;
	    const iso_entry & entry = i->second;
	    isotope_weight_map_t::const_iterator j = i;
	    j++;
	    string atag = du::i_tree_dumpable::TAG;
	    if (j == __composition.end ()) atag = du::i_tree_dumpable::LAST_TAG;
	    out_ << i_tree_dumpable::skip_tag<< atag << "'"<<entry.iso_ptr->get_name ()<< "'" <<" : "<<
	      entry.weight<< " " << (entry.owned? "(owned)": "(not owned)") <<std::endl;
	  }
      } 
    */

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
	    
     
    out_ << indent << i_tree_dumpable::last_tag 
	 << "Locked       : " << (is_locked()? "Yes": "No") << endl;

    return;
    
  }

} // end of namespace mat

// end of material.cc
