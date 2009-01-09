// -*- mode: c++; -*- 
/* physical_volume.cc
 */

#include <geomtools/physical_volume.h>

namespace geomtools {
 
  boost::scoped_ptr<physical_volume::table> physical_volume::g__table;

  const std::string physical_volume::TOP_LOGICAL_NAME = "world";

  /***************************************************/

  physical_volume::table::table ()
  {
  }

  physical_volume::table::~table ()
  {
  }

  bool
  physical_volume::table::has (const std::string & name_) const
  {
    return __physical_dict.find (name_) != __physical_dict.end ();
  }
       	
  const physical_volume & 
  physical_volume::table::get (const std::string & name_) const
  {
    physical_volume_dict::const_iterator found = 
      __physical_dict.find (name_);
    if (found == __physical_dict.end ())
      {
	std::ostringstream message;
	message << "physical_volume::table::get: "
		<< "No physical volume named '" << name_ << "'!";
	throw std::runtime_error (message.str ());
      }
    return * (found->second);
  }
	
  physical_volume & 
  physical_volume::table::get (const std::string & name_)
  {
    physical_volume_dict::iterator found = 
      __physical_dict.find (name_);
    if (found == __physical_dict.end ())
      {
	std::ostringstream message;
	message << "physical_volume::table::get: "
		<< "No physical volume named '" << name_ << "'!";
	throw std::runtime_error (message.str ());
      }
    return * (found->second);
  }
  
  void 
  physical_volume::table::add (physical_volume * physical_volume_)
  {
    physical_volume_dict::const_iterator found = 
      __physical_dict.find (physical_volume_->get_name ());
    if (found != __physical_dict.end ())
      {
	std::cerr << "WARNING: physical_volume::table::add: "
		  << "physical_volume '" << physical_volume_->get_name () 
		  << "' already exists in the table!"
		  << std::endl;
      }
    std::pair<std::string,  boost::shared_ptr<physical_volume> > a_pair;
    a_pair.first = physical_volume_->get_name ();
    a_pair.second.reset (physical_volume_);
    __physical_dict.insert (a_pair);
  }

  /***************************************************/

  physical_volume::table & 
  physical_volume::get_table ()
  {
    if (g__table.get () == 0) 
      {
	physical_volume::table * ptr = new physical_volume::table;
        g__table.reset (ptr);
      }
    return * (g__table.get ());
  }

  const physical_volume & 
  physical_volume::get (const std::string & name_)
  {
    return physical_volume::get_table ().get (name_);
  }

  /***************************************************/
  
  const std::string &
  physical_volume::get_name () const
  {
    return __name;
  }

  void 
  physical_volume::set_name (const std::string & name_)
  {
    __name = name_;
  }

  const placement & 
  physical_volume::get_placement () const
  {
    return __placement;
  }
  
  void 
  physical_volume::set_placement (const placement & placement_)
  {
    __placement = placement_;
  }

  const logical_volume & 
  physical_volume::get_logical () const
  {
    return *__logical;
  }
  
  void 
  physical_volume::set_logical (logical_volume & logical_)
  {
    __logical = &logical_;
  }

  bool
  physical_volume::has_logical () const
  {
    return __logical != 0;
  }

  bool
  physical_volume::has_mother () const
  {
    return __mother != 0;
  }

  const logical_volume & 
  physical_volume::get_mother () const
  {
    return *__mother;
  }
  
  void 
  physical_volume::set_mother (const logical_volume & mother_)
  {
    __mother = &mother_;
  }

  const datatools::utils::properties &
  physical_volume::properties () const
  {
    return __properties;
  }
  
  datatools::utils::properties &
  physical_volume::properties ()
  {
    return __properties;
  }

  physical_volume::physical_volume ()
  {
    __logical = 0;
    __mother = 0;
  }
  
  physical_volume::~physical_volume ()
  {
  }

  void 
  physical_volume::tree_dump (std::ostream & out_,  
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
         << "name      = '" << get_name () << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "placement = '" << get_name () << "'" << std::endl;

    if (__logical != 0)
      {
	out_ << indent << du::i_tree_dumpable::tag 
	     << "logical   = '" << get_logical ().get_name () << "'" << std::endl;
      }
    
    if (__mother != 0)
      {
	out_ << indent << du::i_tree_dumpable::tag 
	     << "mother    = '" << get_mother ().get_name () << "'" << std::endl;
      }

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
  physical_volume::create_physical_volume (const std::string & name_, 
					  const placement & placement_, 
					  const std::string & logical_name_, 
					  const std::string & mother_name_)
  {
    if (!logical_volume::get_table ().has (logical_name_))
      {
	std::ostringstream message;
	message << "physical_volume::create_physical_volume: "
		<< "No logical volume named '" << logical_name_ << "'!";
	throw std::runtime_error (message.str ());
      }
    logical_volume & log = 
      logical_volume::get_table ().get (logical_name_);

    if (!logical_volume::get_table ().has (mother_name_))
      {
	std::ostringstream message;
	message << "physical_volume::create_physical_volume: "
		<< "No mother logical volume named '" << mother_name_ << "'!";
	throw std::runtime_error (message.str ());
      }
    const logical_volume & mother = 
      logical_volume::get_table ().get (mother_name_);

    physical_volume * pv = new physical_volume;
    pv->set_name (name_);
    pv->set_placement (placement_);
    pv->set_logical (log);
    pv->set_mother (mother);
    physical_volume::get_table ().add (pv);
  }


  void 
  physical_volume::print_table_of_physical_volumes (std::ostream & out_)
  {
    out_ << "Table of physical_volumes: " << std::endl;
    const physical_volume_dict & d = physical_volume::get_table ().get_dict ();

    for (physical_volume_dict::const_iterator it = d.begin ();
	 it != d.end ();
	 it++)
      {
	it->second.get ()->tree_dump (out_, it->first);
      }
  }

} // end of namespace geomtools

// end of physical_volume.cc
