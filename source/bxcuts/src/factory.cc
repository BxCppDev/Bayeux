// -*- mode: c++ ; -*- 
/* factory.cc
 */

#include <cuts/factory.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace cuts {

  using namespace std;

  bool factory::g_devel = false;
  
  bool factory::is_debug () const
  {
    return __debug;
  }
  
  void factory::set_debug (bool new_value_)
  {
    __debug = new_value_;
    return;
  }

  void factory::do_register (const cut_creator_t & creator_,
			     const string & cut_id_)
  {
    bool abort_on_error = false;
    cut_creator_dict_t::const_iterator found = _creators.find (cut_id_);
    if (found != _creators.end ())
      {
	ostringstream message;
	message << "factory::do_register: " 
		<< "Cut ID '" << cut_id_ << "' is already used "
		<< "within the factory dictionary!";
	if (abort_on_error)
	  {
	    throw runtime_error (message.str ());
	  }
	else 
	  {
	    message << ' ' << "Overwriting current value...";
	    clog << "WARNING: factory::do_register: " 
		 << message.str () << endl;
	  }
      }
    string cut_id = cut_id_;
    if (cut_id.empty ())
      {
	ostringstream message;
	message << "factory::do_register: " 
		<< "Missing cut ID!";
	throw runtime_error (message.str ());
      }
    _creators [cut_id] = creator_;
    return;
  }

  // ctor:
  factory::factory (bool preload_)
  {
    __debug = false;
    bool devel = g_devel;
    //devel = true;
    if (devel)
      {
	clog << "DEVEL: factory::ctor: Initial cuts..." << endl;
	dump_cuts (clog, "", "DEVEL: ");
      }
    if (preload_)
      {
	clog << "NOTICE: factory::ctor: " 
	     << "Preload the global cuts dictionary!" << endl;
	// preload local vertex generator dictionary with global vertex generator dictionary:
	_creators = i_cut::get_cut_creator_db ().get_dict ();
	/*
	clog << "NOTICE: factory::ctor: " 
	     << "Available cuts after preload:" << endl;
	*/
	dump_cuts (clog, "Available cuts after preload:", "NOTICE: ");
      }
    return;
  }
  
  // dtor:
  factory::~factory ()
  {
    _creators.clear ();
    return;
  }
  
  i_cut * factory::create_cut (const string & cut_id_, 
			       const datatools::utils::properties & cut_configuration_,
			       cut_dict_t & cut_dict_,
			       void * user_)
  {
    using namespace std;
    bool devel = g_devel;
    //devel = true;

    if (devel) 
      {
	cerr << "DEVEL: factory::create_cut: " 
	     << "Cut ID == '" << cut_id_ << "' " << endl;
      }
    // search for the cut's label in the creators dictionary:
    cut_creator_dict_t::iterator found = _creators.find (cut_id_);
    if (found != _creators.end ())
      {
	cut_creator_t a_creator = found->second;
	i_cut * cut = a_creator (cut_configuration_, 
				 &cut_dict_, 
				 user_);
	return cut;
      }
    else
      {
	if (devel) 
	  {
	    cerr << "DEVEL: factory::create_cut: "
		 << "Null cut !" << endl;
	  }
      }
    /*
    else
      {
	i_cut * a_cut = base_factory::create_cut (cut_id_,
	cut_configuration_,user_);
	if (a_cut != 0) 
	  {
	    return (a_cut);
	  }
      }
    */

    return (0);
  }

  void factory::dump_cuts (ostream & out_,
			   const string & title_,
			   const string & indent_) const
  {
    out_ << indent_;
    if (! title_.empty ())
      {
	out_ << title_;
      }
    else
      {
	out_ << "List of cut creators in 'factory::_creators':";
      }
    size_t sz = _creators.size ();
    out_ << " " << sz << " element" << (sz < 2? "": "(s)") << endl;
    size_t count = 0; 
    for (cut_creator_dict_t::const_iterator it = _creators.begin ();
	 it != _creators.end ();
	 it++) 
      {
	count++;
	out_ << indent_;
	if (count == sz) out_ << "`-- "; 
	else out_ << "|-- ";
	out_ << it->first << ": " << hex << (void *) it->second 
	     << dec << endl;
      }
    //out_ << indent_ << "end." << endl;
    return;
  }

  const cut_creator_dict_t & factory::get_creators () const
  {
    return _creators;
  }

  cut_creator_dict_t & factory::get_creators ()
  {
    return _creators;
  }
  
} // end of namespace cuts

// end of factory.cc
