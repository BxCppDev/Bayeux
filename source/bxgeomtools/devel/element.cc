// -*- mode: c++; -*- 
/* element.cc
 */

#include <geomtools/element.h>

namespace geomtools {
 
  boost::scoped_ptr<element::table> element::g__table;

  /***************************************************/

  element::table::table ()
  {
    /*
    std::clog << "NOTICE: element::table::ctor: "
	      << "creation of the table of elements."
	      << std::endl;
    */
  }

  element::table::~table ()
  {
    /*
    std::clog << "NOTICE: element::table::dtor: "
	      << "destruction of the table of elements."
	      << std::endl;
    */
  }

  element *
  element::table::access_element (const std::string & name_)
  {
    element_dict::iterator found = 
      __elements.find (name_);
    if (found == __elements.end ())
      {
	return 0;
      }
    return & (found->second);
  }
	
  const element * 
  element::table::get (const std::string & name_) const
  {
    element_dict::const_iterator found = 
      __elements.find (name_);
    if (found == __elements.end ())
      {
	return 0;
      }
    return & (found->second);
  }

  bool 
  element::table::has (const std::string & name_) const
  {
    return get (name_) != 0;
  }
  
  void 
  element::table::add (const element & element_)
  {
    /*
    std::clog << "NOTICE: element::table::add: "
	      << element_.get_name ()
	      << std::endl;
    */
    element_dict::const_iterator found = 
      __elements.find (element_.get_name ());
    if (found != __elements.end ())
      {
	std::cerr << "WARNING: element::table::add: "
		  << "element '" << element_.get_name () 
		  << "' already exists in the table!"
		  << std::endl;
      }
    __elements[element_.get_name ()] = element_;
  }

  /***************************************************/

  element::table & 
  element::get_table ()
  {
    /*
    std::clog << "NOTICE: element::get_table: "
	      << "access to the table of elements."
	      << std::endl;
    */
    if (g__table.get () == 0) 
      {
	/*
	std::clog << "NOTICE: element::get_table: "
		  << "create the table of elements."
		  << std::endl;
	*/
	element::table * ptr = new element::table;
        g__table.reset (ptr);
      }
    return * (g__table.get ());
  }

  const element * 
  element::get (const std::string & name_)
  {
    return element::get_table ().get (name_);
  }
  
  bool 
  element::is_locked () const
  {
    return __locked;
  }

  void 
  element::lock ()
  {
    __locked = true;
  }
  
  double
  element::get_A () const
  {
    return __A;
  }

  void 
  element::set_A (double a_)
  {
    __A = a_;
  }

  double
  element::get_Z () const
  {
    return __Z;
  }

  void 
  element::set_Z (double z_)
  {
    __Z = z_;
  }

  const std::string &
  element::get_name () const
  {
    return __name;
  }

  void 
  element::set_name (const std::string & name_)
  {
    __name = name_;
  }

  const std::string &
  element::get_symbol () const
  {
    return __symbol;
  }

  void 
  element::set_symbol (const std::string & symbol_)
  {
    __symbol = symbol_;
  }

  const datatools::utils::properties &
  element::properties () const
  {
    return __properties;
  }
  
  datatools::utils::properties &
  element::properties ()
  {
    return __properties;
  }

  void
  element::__set_default_values ()
  {
    set_name ("hydrogen");
    set_symbol ("H");
    set_A (1.00794 * CLHEP::gram / CLHEP::mole);
    set_Z (1.0);
  }

  // ctor/dtor:
  element::element ()
  {
    __set_default_values ();
  }

  element::~element ()
  {
  }

  void
  element::create (const std::string & name_,
		  const std::string & symbol_,
		  double a_,
		  double z_)
  {
    /*
    std::clog << "NOTICE: element::create  '" 
              << name_ << "'"
              << std::endl;
    */
    element elmt;
    elmt.set_name (name_);
    elmt.set_symbol (symbol_);
    elmt.set_A (a_);
    elmt.set_Z (z_);

    element::get_table ().add (elmt);
    
  }

  void 
  element::tree_dump (std::ostream & out_, 
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
         << "symbol  = '"  << get_symbol () << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "A       = "  
         << get_A () / (CLHEP::gram / CLHEP::mole)
	 << " g/mol" << std::endl; 

    out_ << indent << du::i_tree_dumpable::tag 
         << "Z       = "  
         << get_Z () << std::endl; 

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Properties : ";
    if ( __properties.size () == 0 ) { out_ << "<empty>"; }
    out_ << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
      __properties.tree_dump (out_,"",indent_oss.str ());
    }
  }

  void 
  element::print_table_of_elements (std::ostream & out_)
  {
    out_ << "Table of elements: " << std::endl;
    const element_dict & d = element::get_table ().get_dict ();

    for (element_dict::const_iterator it = d.begin ();
	 it != d.end ();
	 it++)
      {
	it->second.tree_dump (out_,it->first);
      }
  }
  
  void 
  element::load_table_of_elements ()
  {
    std::clog << "NOTICE: element::load_table_of_elements: " << std::endl;

    std::string fn = "${GEOMTOOLS_ROOT}/resources/elements.data";
    datatools::utils::fetch_path_with_env (fn);

    std::ifstream elements_db;
    elements_db.open (fn.c_str ());
    if (!elements_db)
      {
	throw std::runtime_error ("element::load_table_of_elements: cannot find elements data file!");
      }
    else
      {
	std::clog << "element::load_table_of_elements: Found '"
		  << fn << "' table file..." << std::endl;
      }
    int line_counter = 0;
    while (elements_db)
      { 
	/*     
	std::clog << "NOTICE: element::load_table_of_elements: while..." 
		  << std::endl;
	*/
	std::string line;
	std::getline (elements_db,line);
	line_counter++;
	/*
	std::clog << "NOTICE: element::load_table_of_elements: while..." 
		  << "line='" << line << "'" << std::endl;
	*/
	if (!elements_db.eof ())
	  {
	    if (!line.empty () && line[0] != '#') 
	      {
		double z, a;
		std::string name, symbol;
		std::istringstream iss (line);
		iss >> z >> symbol >> name >> a; 
		if (iss)
		  {
		    std::clog << "NOTICE: element::load_table_of_elements: " 
			      << "create '" << name 
			      << "'" << std::endl;
		    geomtools::element::create (name, 
						symbol, 
						a * CLHEP::gram / CLHEP::mole, 
						z);
		  }
		else 
		  {
		    std::ostringstream message;
		    message << "element::load_table_of_elements: "
			    << "invalid format at line " 
			    << line_counter;
		    throw std::runtime_error (message.str ());
		  }
	      }
	  }
	else 
	  {
	    break;
	  }
      }
  }      

} // end of namespace geomtools

// end of element.cc
