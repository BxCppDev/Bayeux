// -*- mode: c++ ; -*- 
/* physical_volume.cc
 */

#include <geomtools/physical_volume.h>

namespace geomtools {

  bool physical_volume::is_locked () const
  {
    return __locked;
  }

  void physical_volume::lock ()
  {
    __locked = true;
  }

  void physical_volume::unlock ()
  {
    __locked = false;
  }
 
  const string & physical_volume::get_name () const
  {
    return __name;
  }
  
  void physical_volume::set_name (const string & name_)
  {
    __name = name_;
  }
  
  const properties & physical_volume::parameters () const
  {
    return __parameters;
  }
  
  properties & physical_volume::parameters ()
  {
    return __parameters;
  }

  void physical_volume::set_placement (const placement & p_)
  {
    __placement = p_;
  }
  
  const placement & physical_volume::get_placement () const
  {
    return __placement;
  }
  
  placement & physical_volume::get_placement ()
  {
    return __placement;
  }

  bool physical_volume::has_logical () const
  {
    return __logical != 0;
  }

  void physical_volume::__clear_logical ()
  {
    if (__logical != 0)
      {
	if (__own_logical)
	  {
	    delete __logical;
	  }
	__logical = 0;
	__own_logical = false;
      }
  }
    
  void physical_volume::set_logical (const logical_volume & logical_)
  {
    __clear_logical ();
    __own_logical = false;
    __logical = &logical_;
  }

  void physical_volume::set_logical (const logical_volume * logical_)
  {
    __clear_logical ();
    if (logical_ != 0)
      {
	__own_logical = true;
	__logical = logical_;
      }
  }

  const logical_volume & physical_volume::get_logical () const
  {
    if (! __logical)
      {
	throw runtime_error ("physical_volume::get_logical: Missing logical !");
      }
    return *__logical;
  }

  bool physical_volume::has_mother () const
  {
    return __mother != 0;
  }
    
  void physical_volume::set_mother (const logical_volume & mother_)
  {
    __mother = &mother_;
  }
  
  physical_volume::physical_volume ()
  {
    __locked = false;
    __own_logical = false;
    __logical = 0;
    __mother = 0;
  }
  
  physical_volume::physical_volume (const string & name_)
  {
    __locked = false;
    __own_logical = false;
    __logical = 0;
    __mother = 0;
    set_name (name_);
  }

  physical_volume::physical_volume (const string & name_, 
				    const logical_volume & logical_,
				    const logical_volume & mother_,
				    const placement & placement_)
  {
    __locked = false;
    __own_logical = false;
    __logical = 0;
    set_name (name_);
    set_logical (logical_);    
    set_mother (mother_);    
    set_placement (placement_);    
  }
  
  physical_volume::physical_volume (const string & name_, 
				    const logical_volume * logical_,
				    const logical_volume & mother_,
				    const placement & placement_)
  {
    __locked = false;
    __own_logical = false;
    __logical = 0;
    set_name (name_);
    set_logical (logical_);    
    set_mother (mother_);    
    set_placement (placement_);    
  }
 
  physical_volume::~physical_volume ()
  {
    __locked = false;
    __clear_logical ();
  }

  void physical_volume::tree_dump (ostream & out_, 
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
	<< "Own its logical    = " << (__own_logical? "Yes": "No") << std::endl;
      */
    out_ << indent << i_tree_dumpable::tag 
         << "Logical = ";
    if (has_logical ())
      {
	out_ << "'" << __logical->get_name () << "' " 
	     << (__own_logical? "(owned)": "(not owned)");
      }
    else
      {
	out_ << "<no logical>";
      }
    out_ << std::endl; 

    out_ << indent << i_tree_dumpable::tag 
         << "Mother = ";
    if (has_mother ())
      {
	out_ << "'" << __mother->get_name () << "'";
      }
    else
      {
	out_ << "<no mother>";
      }
    out_ << std::endl; 

    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "Placement : " << endl;
      ostringstream oss_title;
      oss_title << i_tree_dumpable::inherit_tag (inherit_) << "Placement:";
      ostringstream oss_indent;
      oss_indent << indent << i_tree_dumpable::inherit_skip_tag (inherit_);

      /*
	std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << du::i_tree_dumpable::skip_tag;
      */
      __placement.tree_dump (out_, 
			     "", //oss_title.str (), 
			     oss_indent.str (), 
			     inherit_);
    }

  }

} // end of namespace geomtools

// end of physical_volume.cc
