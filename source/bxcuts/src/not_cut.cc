// -*- mode: c++ ; -*- 
/* not_cut.cc
 */

#include <cuts/not_cut.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace cuts {

  using namespace std;

  void not_cut::set_cut (i_cut & cut_)
  {
    if (&cut_ == this)
      {
	ostringstream message;
	message << "not_cut::set_cut: "
		<< "Binary cut with self-reference is not allowed !";
	throw runtime_error (message.str ( ));
      }
    _cut = &cut_;
    return;
  }
    
  void not_cut::set_user_data (void * user_data_)
  {
    if (_cut == 0) 
      {
	throw std::runtime_error ("not_cut::set_user_data: Null 'cut' !");
      }
    _cut->set_user_data (user_data_);
    return;
  }
  
  // ctor:
  not_cut::not_cut ()
  {
    _cut = 0;
  }

  // ctor:
  not_cut::not_cut (i_cut & cut_)
  {
    set_cut (cut_);
  }
  
  // dtor:
  not_cut::~not_cut ()
  {
  }

  bool not_cut::_accept ()
  {
    if (_cut == 0) 
      {
	throw std::runtime_error ("not_cut::_accept: Null 'cut' !");
      }
    if (_cut->accept () == ACCEPT) 
      {
	return (REJECT);
      }
    else 
      {
	return (ACCEPT);
      }
  }

  // static method used within a cut factory:
  i_cut * not_cut::create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_)
  {
    using namespace std;

    i_cut * a_cut = 0;
    string cut_name;
    if (configuration_.has_key ("cut"))
      {
	cut_name = configuration_.fetch_string ("cut");
      }
    else
      {
	throw runtime_error ("xor_cut::create: Missing 'cut' name property !");
      }

    cut_dict_t::iterator it_cut = cut_dict_->find (cut_name);
    if (it_cut == cut_dict_->end())
      {
	ostringstream message;
	message << "xor_cut::create: "
		<< "No cut named '" << cut_name << "' !";
	throw runtime_error (message.str());
      }
    a_cut = it_cut->second;

    // create a new parameterized 'not_cut' instance:
    not_cut * cut_ptr = new not_cut;
    cut_ptr->set_cut (*a_cut);
    return cut_ptr;	
  }

  // register this creator:   
  i_cut::creator_registration<not_cut> not_cut::__CR;
 
  string not_cut::cut_id () const
  {
    return "cuts::not_cut";
  }
  
  cut_creator_t not_cut::cut_creator () const
  {
    return not_cut::create;
  }
  
} // end of namespace cuts

// end of not_cut.cc
