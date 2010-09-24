// -*- mode: c++ ; -*- 
/* accept_cut.cc
 */

#include <cuts/accept_cut.h>

namespace cuts {

  // ctor:
  accept_cut::accept_cut () : i_cut ()
  {
  }
  
  // dtor:
  accept_cut::~accept_cut ()
  {
  }
 
  void accept_cut::set_user_data (void *)
  {
  }

  bool accept_cut::_accept ()
  {
    return i_cut::ACCEPT;
  } 

  // static method used within a cut factory:
  i_cut * accept_cut::create (const properties & configuration_, 
			      cut_dict_t * cut_dict_,
			      void * user_)
  {
    using namespace std;

    // create a new parameterized 'accept_cut' instance:
    accept_cut * ptr = new accept_cut;
    return ptr;	
  }

  // register this creator:   
  i_cut::creator_registration<accept_cut> accept_cut::__CR;
 
  string accept_cut::cut_id () const
  {
    return "cuts::accept_cut";
  }
  
  cut_creator_t accept_cut::cut_creator () const
  {
    return accept_cut::create;
  }
  
} // end of namespace cuts

// end of accept_cut.cc
