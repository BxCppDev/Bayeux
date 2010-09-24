// -*- mode: c++ ; -*- 
/* reject_cut.cc
 */

#include <cuts/reject_cut.h>

namespace cuts {

  // ctor:
  reject_cut::reject_cut () : i_cut ()
  {
  }
  
  // dtor:
  reject_cut::~reject_cut ()
  {
  }
 
  void reject_cut::set_user_data (void *)
  {
  }

  bool reject_cut::_accept ()
  {
    return i_cut::REJECT;
  } 

  // static method used within a cut factory:
  i_cut * reject_cut::create (const properties & configuration_, 
			      cut_dict_t * cut_dict_,
			      void * user_)
  {
    using namespace std;

    // create a new parameterized 'reject_cut' instance:
    reject_cut * ptr = new reject_cut;
    return ptr;	
  }

  // register this creator:   
  i_cut::creator_registration<reject_cut> reject_cut::__CR;
 
  string reject_cut::cut_id () const
  {
    return "cuts::reject_cut";
  }
  
  cut_creator_t reject_cut::cut_creator () const
  {
    return reject_cut::create;
  }
  
} // end of namespace cuts

// end of reject_cut.cc
