// -*- mode: c++ ; -*- 
/* logical_volume.cc
 */

#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  using namespace std;

  bool logical_volume::is_locked () const
  {
    return __locked;
  }

  void logical_volume::lock ()
  {
    __locked = true;
  }

  void logical_volume::unlock ()
  {
    __locked = false;
  }
 
  const string & logical_volume::get_name () const
  {
    return __name;
  }
  
  void logical_volume::set_name (const string & name_)
  {
    __name = name_;
  }
  
  const properties & logical_volume::parameters () const
  {
    return __parameters;
  }
  
  properties & logical_volume::parameters ()
  {
    return __parameters;
  }

  bool logical_volume::has_shape () const
  {
    return __shape != 0;
  }

  void logical_volume::__clear_shape ()
  {
    if (__shape != 0)
      {
	if (__own_shape)
	  {
	    delete __shape;
	  }
	__shape = 0;
	__own_shape = false;
      }
  }

  void logical_volume::set_shape (const i_shape_3d & shape_)
  {
    __clear_shape ();
    __own_shape = false;
    __shape = &shape_;
  }

  void logical_volume::set_shape (const i_shape_3d * shape_)
  {
    __clear_shape ();
    if (shape_ != 0)
      {
	__own_shape = true;
	__shape = shape_;
      }
  }

  const i_shape_3d & logical_volume::get_shape () const
  {
    if (! __shape)
      {
	throw runtime_error ("logical_volume::get_shape: Missing shape !");
      }
    return *__shape;
  }
 
  bool logical_volume::has_material_ref () const
  {
    if (material::has_key (__parameters, material::MATERIAL_REF_PROPERTY))
      {
	return (__parameters.is_string  (material::make_key (material::MATERIAL_REF_PROPERTY)));
      }
    return false;
  }
 
  string logical_volume::get_material_ref () const
  {
    if (has_material_ref ())
      {
	return (__parameters.fetch_string  (material::make_key (material::MATERIAL_REF_PROPERTY)));
      }
    return material::MATERIAL_REF_UNKWOWN;
  }

  void logical_volume::set_material_ref (const string & material_ref_)
  {
    string mr = material_ref_;
    if (mr.empty ())
      {
	mr = material::MATERIAL_REF_DEFAULT;
      }
    __parameters.update (material::make_key (material::MATERIAL_REF_PROPERTY), material_ref_);
  }

  logical_volume::logical_volume ()
  {
    __locked = false;
    __own_shape = false;
    __shape = 0;
  }

  logical_volume::logical_volume (const string & name_)
  {
    __locked = false;
    __own_shape = false;
    __shape = 0;
    set_name (name_);
  }

  logical_volume::logical_volume (const string & name_, 
				  const i_shape_3d & shape_)
  {
    __locked = false;
    __own_shape = false;
    __shape = 0;
    set_name (name_);
    set_shape (shape_);
  }

  logical_volume::logical_volume (const string & name_, 
				  const i_shape_3d * shape_)
  {
    __locked = false;
    __own_shape = false;
    __shape = 0;
    set_name (name_);
    set_shape (shape_);    
  }
  
  logical_volume::~logical_volume ()
  {
    __locked = false;
    __clear_shape ();
  }
 
  bool logical_volume::has_physical (const string & name_) const
  {
    return (__physicals.find (name_) != __physicals.end ());
  }

  const logical_volume::physicals_col_t & 
  logical_volume::get_physicals () const
  {
    return __physicals;
  }

  const physical_volume & logical_volume::get_physical (const string & name_) const
  {
    physicals_col_t::const_iterator found = __physicals.find (name_);
    if (found == __physicals.end ())
      {
	ostringstream message;
	message << "logical_volume::get_physical: "
		<< "Name '" << name_ << "' is not used !";
	throw runtime_error (message.str ());
      }
    return *(found->second);
  }
    
  void logical_volume::add_physical (const physical_volume & phys_,
				     const string & name_)
  {
    if (__physicals.find (name_) != __physicals.end ())
      {
	ostringstream message;
	message << "logical_volume::add_physical: "
		<< "name '" << name_ << "' is already used !";
	throw runtime_error (message.str ());
      }
    string name;
    if (name_.empty ())
      {
	name = phys_.get_name ();
      }
    else
      {
	name = name_;
      }
    if (name.empty ())
      {
	throw runtime_error ("logical_volume::add_physical: Missing physical's name !");
      }
    __physicals[name] = &phys_;
    return;
  }

  void logical_volume::tree_dump (ostream & out_, 
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
	 << "Name       : \"" << __name << "\"" << endl;

    out_ << indent << i_tree_dumpable::tag 
	 << "Locked     : " << (__locked? "Yes": "No") << endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Parameters : ";
      if ( __parameters.size () == 0) 
        {
          out_ << "<empty>"; 
        }
      out_ << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        __parameters.tree_dump (out_,"",indent_oss.str ());
      }
    }
    
    {
      out_ << indent << i_tree_dumpable::tag 
	   << "Shape : ";
      if (has_shape ())
	{
	  out_ << "'" << __shape->get_shape_name () << "' " 
	       << (__own_shape? "(owned)": "(not owned)");
	}
      else
	{
	  out_ << "<no shape>";
	}
      out_ << endl; 
      {
	ostringstream indent_oss;
	indent_oss << indent;
	indent_oss << du::i_tree_dumpable::skip_tag;
	__shape->tree_dump (out_,"",indent_oss.str ());
      }
    }
      
    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "Physicals : ";
      if (__physicals.size ())
	{
	  for (physicals_col_t::const_iterator i = __physicals.begin ();
	       i != __physicals.end ();
	       i++)
	    {
	      out_ << i->first << ' ';
	    }
	  out_ << endl; 
	}
      else
	{
	  out_ << "<none>" << endl;
	}
    }

    return;
  }
  
} // end of namespace geomtools

// end of logical_volume.cc
