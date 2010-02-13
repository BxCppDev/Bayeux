// -*- mode: c++ ; -*- 
/* logical_volume.cc
 */

#include <geomtools/logical_volume.h>

namespace geomtools {

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
        out_ << indent << title_ << std::endl;
      }
    out_ << indent << i_tree_dumpable::tag 
	 << "Name    = " << __name << std::endl;

    out_ << indent << i_tree_dumpable::tag 
	 << "Locked    = " << (__locked? "Yes": "No") << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
	 << "Parameters : ";
      if ( __parameters.size () == 0) 
        {
          out_ << "<empty>"; 
        }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        __parameters.tree_dump (out_,"",indent_oss.str ());
      }
      /*
    out_ << indent << i_tree_dumpable::tag 
	 << "Own its shape    = " << (__own_shape? "Yes": "No") << std::endl;
      */
    out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
         << "Shape = ";
    if (has_shape ())
      {
	out_ << "'" << __shape->get_shape_name () << "' " 
	     << (__own_shape? "(owned)": "(not owned)");
      }
    else
      {
	out_ << "<no shape>";
      }
    out_ << std::endl; 
    
  }
  
} // end of namespace geomtools

// end of logical_volume.cc
