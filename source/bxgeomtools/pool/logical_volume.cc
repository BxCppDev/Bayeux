// -*- mode: c++; -*- 
/* logical_volume.cc
 */

#include <geomtools/logical_volume.h>

namespace geomtools {
 
  boost::scoped_ptr<logical_volume::table> logical_volume::g__table;

  /***************************************************/

  logical_volume::table::table ()
  {
  }

  logical_volume::table::~table ()
  {
  }

  bool
  logical_volume::table::has (const std::string & name_) const
  {
    return __logical_dict.find (name_) != __logical_dict.end ();
  }
	
  logical_volume & 
  logical_volume::table::get (const std::string & name_)
  {
    logical_volume_dict::iterator found = 
      __logical_dict.find (name_);
    if (found == __logical_dict.end ())
      {
	std::ostringstream message;
	message << "logical_volume::table::get: "
		<< "No logical volume named '" << name_ << "'!";
	throw std::runtime_error (message.str ());
      }
    return * (found->second);
  }
  
  const logical_volume &
  logical_volume::table::get (const std::string & name_) const
  {
    logical_volume_dict::const_iterator found = 
      __logical_dict.find (name_);
    if (found == __logical_dict.end ())
      {
	std::ostringstream message;
	message << "logical_volume::table::get: "
		<< "No logical volume named '" << name_ << "'!";
	throw std::runtime_error (message.str ());
      }
    return * (found->second);
  }
  
  void 
  logical_volume::table::add (logical_volume * logical_volume_)
  {
    logical_volume_dict::const_iterator found = 
      __logical_dict.find (logical_volume_->get_name ());
    if (found != __logical_dict.end ())
      {
	std::cerr << "WARNING: logical_volume::table::add: "
		  << "logical_volume '" << logical_volume_->get_name () 
		  << "' already exists in the table!"
		  << std::endl;
      }
    std::pair<std::string,  boost::shared_ptr<logical_volume> > a_pair;
    a_pair.first = logical_volume_->get_name ();
    a_pair.second.reset (logical_volume_);
    __logical_dict.insert (a_pair);
  }

  /***************************************************/

  logical_volume::table & 
  logical_volume::get_table ()
  {
    if (g__table.get () == 0) 
      {
	logical_volume::table * ptr = new logical_volume::table;
        g__table.reset (ptr);
      }
    return * (g__table.get ());
  }

  const logical_volume & 
  logical_volume::get (const std::string & name_)
  {
    return logical_volume::get_table ().get (name_);
  }

  /*****************************************************/
  
  bool 
  logical_volume::is_locked () const
  {
    return __locked;
  }

  void 
  logical_volume::lock ()
  {
    __locked = true;
  }
  
  void 
  logical_volume::unlock ()
  {
    __locked = false;
  }
  
  const std::string &
  logical_volume::get_name () const
  {
    return __name;
  }

  void 
  logical_volume::set_name (const std::string & name_)
  {
    __name = name_;
  }

  bool 
  logical_volume::has_shape () const
  {
    if (__own_shape.get () != 0) return true;
    if (__ext_shape != 0) return true;
    return false;
  }

  const i_shape_3d &
  logical_volume::get_shape () const
  {
    if (__own_shape.get () != 0)
      {
	return * (__own_shape.get ());
      }
    if (__ext_shape == 0) 
      {
	throw std::runtime_error ("logical_volume::get_shape: no available shape object!");
      }
    return *__ext_shape;
  }

  i_shape_3d &
  logical_volume::get_shape ()
  {
    /*
    if (__own_shape.get () != 0)
      {
	return * (__own_shape.get ());
      }
    if (__ext_shape == 0) 
      {
	throw std::runtime_error ("logical_volume::get_shape: no available non-const shape object!");
      }
    return *__ext_shape;
    */    
    if (__own_shape.get () == 0)
      { 
	throw std::runtime_error ("logical_volume::get_shape: no available non-const shape object!");
      }
    return * (__own_shape.get ());
  }

  void 
  logical_volume::set_shape (const i_shape_3d & shape_)
  {
    if (__own_shape.get () != 0)
      {
	__own_shape.reset ();
      }
    __ext_shape = &shape_;
  }

  void 
  logical_volume::set_shape (i_shape_3d * shape_)
  {
    __ext_shape = 0;
    if (shape_ == 0)
      {
	throw std::runtime_error ("logical_volume::set_shape: NULL shape pointer!");
      }
    __own_shape.reset (shape_);
  }

  const material &
  logical_volume::get_material () const
  {
    return *__material;
  }

  void 
  logical_volume::set_material (const std::string & material_name_)
  {
    if (! material::get_table ().has (material_name_))
      {
	std::ostringstream message;
	message << "logical_volume::set_material: "
		<< "No material named '" << material_name_ << "'!";
	throw std::runtime_error (message.str ());	
      }
    __material = &material::get (material_name_);
  }

  const datatools::utils::properties &
  logical_volume::properties () const
  {
    return __properties;
  }
  
  datatools::utils::properties &
  logical_volume::properties ()
  {
    return __properties;
  }

  void
  logical_volume::__set_default_values ()
  {
    
    set_name ("logical_volume");
    __material = 0;
    set_shape (0);
  }

  // ctor/dtor:
  logical_volume::logical_volume ()
  {
    __own_shape.reset ();
    __ext_shape = 0;
    __set_default_values ();
  }

  logical_volume::~logical_volume ()
  {
    __own_shape.reset ();
  }

  void
  logical_volume::create (const std::string & name_, 
			 const std::string & material_name_, 
			 const i_shape_3d & shape_, 
			 const datatools::utils::properties & properties_)
  {
    
    std::clog << "NOTICE: logical_volume::create  '" 
              << name_ << "'"
              << std::endl;
    
    logical_volume * log = new logical_volume;
    log->set_name (name_);
    log->set_material (material_name_);
    log->set_shape (shape_);
    log->properties () = properties_;
    logical_volume::get_table ().add (log); 
  }

  void 
  logical_volume::tree_dump (std::ostream & out_,  
			    const std::string & title_, 
			    const std::string & indent_, 
			    bool inherit_) const
  {
    std::string indent;
    if ( !indent_.empty () ) indent = indent_;
    if ( !title_.empty () ) 
      {
	out_ << indent << title_ << std::endl;
      }
    namespace du = datatools::utils;

    out_ << indent << du::i_tree_dumpable::tag 
         << "name    = '" << get_name () << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "material    = '" << get_material ().get_name () << "'" 
	 << " [" << &get_material () << "]" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "shape       = " 
	 << " [" << &get_shape () << "]" << std::endl;

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Properties : ";
    if ( __properties.size () == 0 ) { out_ << "<empty>"; }
    out_ << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
      __properties.tree_dump (out_, "", indent_oss.str ());
    }
  }

  void 
  logical_volume::print_table_of_logical_volumes (std::ostream & out_)
  {
    out_ << "Table of logical_volumes: " << std::endl;
    const logical_volume_dict & d = logical_volume::get_table ().get_dict ();

    for (logical_volume_dict::const_iterator it = d.begin ();
	 it != d.end ();
	 it++)
      {
	it->second.get ()->tree_dump (out_, it->first);
      }
  }

} // end of namespace geomtools

// end of logical_volume.cc
