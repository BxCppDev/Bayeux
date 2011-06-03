// -*- mode: c++ ; -*- 
/* multi_cut.cc
 */

#include <cuts/multi_cut.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace cuts {

  using namespace std;

  void multi_cut::add_cut (i_cut & cut_)
  {
    cuts_col_t::iterator found = find (__cuts.begin (),
				       __cuts.end (),
				       &cut_);
    if (found !=  __cuts.end ())
      {
	ostringstream message;
	message << "multi_cut::set_cut: "
		<< "Cut is already registered !";
	cerr << "WARNING: " << message.str () << endl;
	return;
      }
    if (&cut_ == this)
      {
	ostringstream message;
	message << "multi_cut::set_cut: "
		<< "Multi cut with self-reference is multi allowed !";
	throw runtime_error (message.str ( ));
      }
    __cuts.push_back (&cut_);
    return;
  }
    
  void multi_cut::set_user_data (void * user_data_)
  {
    if (__cuts.size () == 0) 
      {
	throw std::runtime_error ("multi_cut::set_user_data: Missing cuts !");
      }
    for (cuts_col_t::iterator i = __cuts.begin ();
	 i !=  __cuts.end ();
	 i++)
      {
	(*i)->set_user_data (user_data_);
      }
    return;
  }
  
  // ctor:
  multi_cut::multi_cut ()
  {
  }

  // dtor:
  multi_cut::~multi_cut ()
  {
    __cuts.clear ();
  }

  bool multi_cut::_accept ()
  {
    if (__cuts.size () == 0) 
      {
	throw std::runtime_error ("multi_cut::_accept: Missing cuts !");
      }
    
    for (cuts_col_t::iterator i = __cuts.begin ();
	 i !=  __cuts.end ();
	 i++)
      {
	i_cut * a_cut = *i;
	if (a_cut->accept () == REJECT)
	  {
	    return REJECT;
	  }
      }
    return (ACCEPT);
  }

  // static method used within a cut factory:
  i_cut * multi_cut::create (const properties & configuration_, 
			     cut_dict_t * cut_dict_,
			     void * user_)
  {
    using namespace std;

    i_cut * cut = 0;
    vector<string> cut_names;
    if (configuration_.has_key ("cuts"))
      {
	configuration_.fetch ("cuts", cut_names);
      }
    else
      {
	throw runtime_error ("multi_cut::create: Missing 'cuts' name property !");
      }

    // create a new parameterized 'multi_cut' instance:
    multi_cut * cut_ptr = new multi_cut;

    for (int i = 0; i < cut_names.size (); i++)
      {
	const string & cut_name = cut_names[i];
	cut_dict_t::iterator it_cut = cut_dict_->find (cut_name);
	if (it_cut == cut_dict_->end ())
	  {
	    delete cut_ptr;
	    ostringstream message;
	    message << "multi_cut::create: "
		    << "No cut named '" << cut_name << "' !";
	    throw runtime_error (message.str());
	  }
	i_cut * a_cut = it_cut->second;
  	cut_ptr->add_cut (*a_cut);
      }

    return cut_ptr;	
  }

  // register this creator:   
  i_cut::creator_registration<multi_cut> multi_cut::__CR;
 
  string multi_cut::cut_id () const
  {
    return "cuts::multi_cut";
  }
  
  cut_creator_t multi_cut::cut_creator () const
  {
    return multi_cut::create;
  }
  
} // end of namespace cuts

// end of multi_cut.cc
