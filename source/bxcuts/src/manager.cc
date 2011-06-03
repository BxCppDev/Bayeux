// -*- mode: c++ ; -*- 
/* manager.cc
 */

#include <cuts/manager.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

//#include <cuts/factory.h>

namespace cuts {

  using namespace std;

  bool manager::is_debug () const
  {
    return __debug;
  }

  void manager::set_debug (bool d_)
  {
    __debug = d_;
    return;
  }

  manager::manager ()
  {
    __debug = false;
    __factory = 0;
    return;
  }

  manager::~manager ()
  {
    reset ();
    return;
  }

  void manager::initialize (const datatools::utils::multi_properties & config_)
  {
    using namespace datatools::utils;

    if (__factory == 0)
      {
	__factory = new factory ();
      }
    
    for (multi_properties::entries_ordered_col_t::const_iterator i 
	   = config_.ordered_entries ().begin ();
	 i != config_.ordered_entries ().end ();
	 i++)
      {
	multi_properties::entry * e = *i;
	const string & cut_name = e->get_key ();

	if (is_debug ())
	  {
	    ostringstream message;
	    message << "manager::initialize: "
		    << "Trying to create a cut with name '" << cut_name << "'...";
	    clog << "DEBUG: " << message.str () << endl;
	  }
	if (__cuts.find (cut_name) != __cuts.end ())
	  {
	    ostringstream message;
	    message << "manager::initialize: "
		    << "Cut with name '" << cut_name << "' already exists !";
	    throw runtime_error (message.str ());
	  }
	const string & cut_id = e->get_meta ();
	const properties & cut_props = e->get_properties ();
	void * user = 0;
	i_cut * a_cut = 0;
	a_cut = __factory->create_cut (cut_id, 
				       cut_props,
				       __cuts,
				       user);
	// add this cut in the dictionary of cuts:
	if (a_cut != 0)
	  {
	    if (is_debug ())
	      {
		ostringstream message;
		message << "manager::initialize: "
			<< "Adding a cut names '" << cut_name << "' with type '"
			<< cut_id << "'...";
		clog << "DEBUG: " << message.str () << endl;
	      }
	    __cuts[cut_name] = a_cut;
	  }
      }
    return;
  }

  void manager::reset ()
  {
    if (__cuts.size () > 0)
      {
	for (cut_dict_t::iterator i = __cuts.begin ();
	     i != __cuts.end ();
	     i++)
	  {
	    if (i->second != 0)
	      {
		delete i->second;
	      }
	  }
	__cuts.clear ();
      }

    if (__factory != 0)
      {
	delete __factory;
	__factory = 0;
      }
    return;
  }

  bool manager::has_cut (const string & cut_name_) const
  {
    return __cuts.find (cut_name_) != __cuts.end();
  }

  i_cut * manager::get_cut (const string & cut_name_)
  {
    cut_dict_t::iterator found = __cuts.find (cut_name_);
    if (found == __cuts.end())
      {
	ostringstream message;
	message << "manager::get_cut: "
		<< "No cut with name '" << cut_name_ << "' !";
	throw runtime_error (message.str ());    
	return 0;
      }
    return (found->second);
  }

} // end of namespace cuts

// end of manager.cc
