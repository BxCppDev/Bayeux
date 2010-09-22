// -*- mode: c++ ; -*- 
/* manager.cc
 */

#include <materials/manager.h>

namespace mat {
  
  bool manager::is_debug () const
  {
    return __debug;
  }
  
  void manager::set_debug (bool new_value_)
  {
    __debug = new_value_;
    return;
  }
  
  bool manager::is_locked () const
  {
    return __locked;
  }
  
  const isotope_dict_t & manager::get_isotopes () const
  {
    return __isotopes;
  }
  
  const element_dict_t & manager::get_elements () const
  {
    return __elements;
  }
  
  const material_dict_t & manager::get_materials () const
  {
    return __materials;
  }
  
  
  // ctor:
  manager::manager ()
  {
    __debug = false;
    __locked = false;
    return;
  }
  
  // dtor:
  manager::~manager ()
  {
    __materials.clear ();
    __elements.clear ();
    __isotopes.clear ();
    __ordered_materials.clear ();
    return;
  }

  void manager::lock ()
  {
    __locked = true;
    return;
  }

  void manager::unlock ()
  {
    __locked = false;
    return;
  }
  
  void manager::load (const datatools::utils::multi_properties & config_)
  {
    if (is_locked ())
      {
	throw runtime_error ("mat::manager::load: Manager is locked !");
      }
    
   for (multi_properties::entries_ordered_col_t::const_iterator i 
           = config_.ordered_entries ().begin ();
         i != config_.ordered_entries ().end ();
         i++)
      {
        multi_properties::entry * e = *i;
        const string & name = e->get_key ();
        const string & type = e->get_meta ();
        const datatools::utils::properties & props = e->get_properties ();
   
	if (__debug)
	  {
	    clog << "DEBUG: " << "manager::load: " << "Name = '" << name << "'" << endl;
	    clog << "DEBUG: " << "manager::load: " << "Type = '" << type << "'" << endl;
	  }
	if (type == "isotope")
	  {
	    if (__isotopes.find (name) != __isotopes.end ())
	      {
		ostringstream message;
		message << "manager::load: "
			<< "Isotope with name '" << name << "' already exists !";
		throw runtime_error (message.str ());
	      }
	    isotope * iso = __creator.create_isotope (name, props);
	    __isotopes[iso->get_name ()] = mat::smart_ref<isotope> ();
	    __isotopes[iso->get_name ()].set_ref (iso);
	    if (__debug) clog << "DEBUG: " << "manager::load: " << "Add new isotope = '" << iso->get_ZAI_name () << "'" << endl;
	  }
	else if (type == "element")
	  {
	    if (__elements.find (name) != __elements.end ())
	      {
		ostringstream message;
		message << "manager::load: "
			<< "Element with name '" << name << "' already exists !";
		throw runtime_error (message.str ());
	      }
	    element * elmt = __creator.create_element (name, props, __isotopes);
	    __elements[elmt->get_name ()] = mat::smart_ref<element> ();
	    __elements[elmt->get_name ()].set_ref (elmt);
	    if (__debug) clog << "DEBUG: " << "manager::load: " << "Add new element = '" << elmt->get_name () << "'" << endl;
	  }
	else if (type == "material")
	  {
	    if (__materials.find (name) != __materials.end ())
	      {
		ostringstream message;
		message << "manager::load: "
			<< "Material with name '" << name << "' already exists !";
		throw runtime_error (message.str ());
	      }
	    material * matl = __creator.create_material (name, 
							 props, 
							 __elements, 
							 __materials);
	    __materials[matl->get_name ()] = mat::smart_ref<material> ();
	    __materials[matl->get_name ()].set_ref (matl);
	    if (__debug) clog << "DEBUG: " << "manager::load: " << "Add new material = '" << matl->get_name () << "'" << endl;
	    __ordered_materials.push_back (matl->get_name ());
	  }
	else if (type == "alias")
	  {
	    if (__materials.find (name) != __materials.end ())
	      {
		ostringstream message;
		message << "manager::load: "
			<< "Material with name '" << name << "' already exists !";
		throw runtime_error (message.str ());
	      }
	    if (! props.has_key ("alias"))
	      {
		ostringstream message;
		message << "manager::load: "
			<< "Missing property 'alias' for a material alias !";
		throw runtime_error (message.str ());		
	      }
	    string alias_material = props.fetch_string ("alias");
	    material_dict_t::iterator found = __materials.find (alias_material);
	    if (found == __materials.end ())
	      {
		ostringstream message;
		message << "manager::load: "
			<< "Material to be aliased with name '" << alias_material << "' does not exist !";
		throw runtime_error (message.str ());
	      }
	    __materials[name] = mat::smart_ref<material> ();
	    __materials[name].set_ref (found->second.get_ref ());
	    __ordered_materials.push_back (name);
	    if (__debug) clog << "DEBUG: " << "manager::load: " << "Add new material alias = '" << name << "'" << endl;
	  }

      } // for

    return;
  }

  void manager::tree_dump (ostream & out_,  
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
	 << "Debug : " << is_debug () << endl;   
	 
    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Isotopes : ";
      if ( __isotopes.size () == 0)
	{
	  out_ << "<empty>" << endl; 
	}
      else
        {
	  out_ << endl;
	  for (isotope_dict_t::const_iterator i = __isotopes.begin ();
	        i != __isotopes.end ();
	       i++)
	    {
	      ostringstream indent_oss;
	      indent_oss << indent;
	      indent_oss << du::i_tree_dumpable::skip_tag;
	      isotope_dict_t::const_iterator j = i;
	      j++;
	      out_ << indent << du::i_tree_dumpable::skip_tag;;
	      if (j != __isotopes.end ()) 
		{
		  out_ << du::i_tree_dumpable::tag;
		  indent_oss << du::i_tree_dumpable::skip_tag;;
		}
	      else 
		{
		  out_ << du::i_tree_dumpable::last_tag;
		  indent_oss << du::i_tree_dumpable::last_skip_tag;;
		}
	      out_ << "Isotope '" << i->first << "'" << endl;
	      i->second.get_ref ().tree_dump (out_, "", indent_oss.str ());
	    }
        }
    }  
   
    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Elements : ";
      if ( __elements.size () == 0)
	{
	  out_ << "<empty>" << endl; 
	}
      else
        {
	  out_ << endl;
	  for (element_dict_t::const_iterator i = __elements.begin ();
	        i != __elements.end ();
	       i++)
	    {
	      ostringstream indent_oss;
	      indent_oss << indent;
	      indent_oss << du::i_tree_dumpable::skip_tag;
	      element_dict_t::const_iterator j = i;
	      j++;
	      out_ << indent << du::i_tree_dumpable::skip_tag;;
	      if (j != __elements.end ()) 
		{
		  out_ << du::i_tree_dumpable::tag;
		  indent_oss << du::i_tree_dumpable::skip_tag;;
		}
	      else 
		{
		  out_ << du::i_tree_dumpable::last_tag;
		  indent_oss << du::i_tree_dumpable::last_skip_tag;;
		}
	      out_ << "Element '" << i->first << "'" << endl;
	      i->second.get_ref ().tree_dump (out_, "", indent_oss.str ());
	    }
        }
    }     
	    
    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Materials : ";
      if ( __materials.size () == 0)
	{
	  out_ << "<empty>" << endl; 
	}
      else
        {
	  out_ << endl;
	  for (material_dict_t::const_iterator i = __materials.begin ();
	        i != __materials.end ();
	       i++)
	    {
	      ostringstream indent_oss;
	      indent_oss << indent;
	      indent_oss << du::i_tree_dumpable::skip_tag;
	      material_dict_t::const_iterator j = i;
	      j++;
	      out_ << indent << du::i_tree_dumpable::skip_tag;;
	      if (j != __materials.end ()) 
		{
		  out_ << du::i_tree_dumpable::tag;
		  indent_oss << du::i_tree_dumpable::skip_tag;;
		}
	      else 
		{
		  out_ << du::i_tree_dumpable::last_tag;
		  indent_oss << du::i_tree_dumpable::last_skip_tag;;
		}
	      out_ << "Material '" << i->first << "'" << endl;
	      i->second.get_ref ().tree_dump (out_, "", indent_oss.str ());
	    }
        }
    }     
	    
	    
    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Materials (ordered): ";
      if ( __ordered_materials.size () == 0)
	{
	  out_ << "<empty>" << endl; 
	}
      else
        {
	  out_ << endl;
	  for (list<string>::const_iterator i = __ordered_materials.begin ();
	        i != __ordered_materials.end ();
	       i++)
	    {
	      ostringstream indent_oss;
	      indent_oss << indent;
	      indent_oss << du::i_tree_dumpable::skip_tag;
	      list<string>::const_iterator j = i;
	      j++;
	      out_ << indent << du::i_tree_dumpable::skip_tag;;
	      if (j != __ordered_materials.end ()) 
		{
		  out_ << du::i_tree_dumpable::tag;
		  indent_oss << du::i_tree_dumpable::skip_tag;;
		}
	      else 
		{
		  out_ << du::i_tree_dumpable::last_tag;
		  indent_oss << du::i_tree_dumpable::last_skip_tag;;
		}
	      out_ << "Material '" << *i << "'" << endl;
	    }
        }
    }     
     
    out_ << indent << i_tree_dumpable::last_tag 
	 << "Locked       : " << (is_locked()? "Yes": "No") << endl;

    return;
    
  }

  void manager::export_gdml (ostream & out_) const
  {
    geomtools::gdml_writer GW;

    for (isotope_dict_t::const_iterator i = __isotopes.begin ();
	 i != __isotopes.end ();
	 i++)
      {
	const isotope & iso = i->second.get_ref ();
	GW.add_isotope (i->first, 
			iso.get_Z (),
			iso.get_A (),
			iso.get_mass ()); // / (CLHEP::g /CLHEP::mole));
		     
      }

    for (element_dict_t::const_iterator i = __elements.begin ();
	 i != __elements.end ();
	 i++)
      {
	const element & elmt = i->second.get_ref ();
	map<string, double> fractions;
	for (isotope_weight_map_t::const_iterator j 
	       = elmt.get_composition ().begin ();
	     j != elmt.get_composition ().end ();
	     j++)
	  {
	    fractions[j->first] = j->second.weight;
	  }
	GW.add_element (i->first, fractions);
      }

    out_ << GW.get_stream (geomtools::gdml_writer::MATERIALS_SECTION).str ();
    return;
  }
  
} // end of namespace mat

// end of manager.cc
